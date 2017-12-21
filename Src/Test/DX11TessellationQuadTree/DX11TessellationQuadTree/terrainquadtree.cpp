
#include "stdafx.h"
#include "terrainquadtree.h"

using namespace graphic;

// Quad Tree Traverse Stack Memory (no multithread safe!!)
struct sData
{
	sRectf rect;
	int level;
	sQuadTreeNode<cTerrainQuadTree::sQuadData> *node;
};
sData g_stack[1024];


cTerrainQuadTree::cTerrainQuadTree()
	: m_isShowQuadTree(false)
{
}

cTerrainQuadTree::cTerrainQuadTree(sRectf &rect)
	: m_rect(rect)
{
}

cTerrainQuadTree::~cTerrainQuadTree()
{
}


bool cTerrainQuadTree::Create(graphic::cRenderer &renderer)
{
	sVertexTex vtx;
	vtx.p = Vector3(0, 0, 0);
	vtx.u = 0;
	vtx.v = 0;
	m_vtxBuff.Create(renderer, 1, sizeof(sVertexTex), &vtx);

	if (!m_shader.Create(renderer, "../media/shader11/tess-pos-quadtree.fxo", "Unlit"
		, eVertexType::POSITION | eVertexType::TEXTURE0))
	{
		return false;
	}

	if (!m_cbTessellation.Create(renderer))
		return false;

	m_cbTessellation.m_v->tessellationAmount = 1.f;

	return true;
}


void cTerrainQuadTree::Render(graphic::cRenderer &renderer
	, const graphic::cFrustum &frustum
	, const int limitLevel, const int level
	, const Ray &ray 
)
{
	BuildQuadTree(frustum);
	CalcSeamlessLevel();
	CalcSeamlessLevel();

	if (m_isShowQuadTree)
		RenderQuad(renderer, ray);
	else
		RenderTessellation(renderer);
}


void cTerrainQuadTree::BuildQuadTree(const graphic::cFrustum &frustum)
{
	m_qtree.Clear();
	m_qtree.Insert(new sQuadTreeNode<sQuadData>);

	int sp = 0;
	g_stack[sp++] = { m_rect, 15, m_qtree.m_root };

	while (sp > 0)
	{
		const sRectf rect = g_stack[sp - 1].rect;
		const int lv = g_stack[sp - 1].level;
		sQuadTreeNode<sQuadData> *parentNode = g_stack[sp - 1].node;
		--sp;

		const float hw = rect.Width() / 2.f;
		const float hh = rect.Height() / 2.f;
		const Vector3 center = Vector3(rect.Center().x, 0, rect.Center().y);
		const float radius = sqrt(hw*hw + hh*hh);

		cBoundingSphere bsphere;
		bsphere.SetBoundingSphere(center, radius);
		if (!frustum.IsInSphere(bsphere))
			continue;

		const Vector3 eyePos(frustum.m_pos.x, 0, frustum.m_pos.z);
		const float distance = max(0, center.Distance(eyePos) - radius);
		const int curLevel = GetLevel(distance);
		const bool isMinSize = hw < 5.f;

		if ((isMinSize || (lv <= curLevel)))
		{
			continue;
		}

		m_qtree.InsertChildren(parentNode);

		g_stack[sp++] = { sRectf::Rect(rect.left, rect.top, hw, hh), lv - 1, parentNode->children[0] };
		g_stack[sp++] = { sRectf::Rect(rect.left + hw, rect.top, hw, hh), lv - 1, parentNode->children[1] };
		g_stack[sp++] = { sRectf::Rect(rect.left, rect.top + hh, hw, hh), lv - 1, parentNode->children[2] };
		g_stack[sp++] = { sRectf::Rect(rect.left + hw, rect.top + hh, hw, hh), lv - 1, parentNode->children[3] };

		assert(sp < 1024);
	}
}


void cTerrainQuadTree::RenderTessellation(graphic::cRenderer &renderer)
{
	m_shader.SetTechnique("Unlit");
	m_shader.Begin();
	m_shader.BeginPass(renderer, 0);

	int sp = 0;
	g_stack[sp++] = { m_rect, 15, m_qtree.m_root };

	while (sp > 0)
	{
		sQuadTreeNode<sQuadData> *node = g_stack[sp - 1].node;
		--sp;

		// leaf node?
		if (!node->children[0])
		{
			const sRectf rect = m_qtree.GetNodeRect(node);

			Transform tfm;
			tfm.pos.x = rect.left;
			tfm.pos.z = rect.top;
			tfm.pos.y = 0.f;
			renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM());
			renderer.m_cbPerFrame.Update(renderer);

			//  Z axis
			// /|\
			//  |
			//  |
			//  |
			// -----------> X axis
			//
			//	 Counter Clock Wise Tessellation Quad
			//        North
			//  * ------3--------*
			//  |                |
			//  |                |
			//  0 West           2 East
			//  |                |
			//  |                |
			//  * ------1--------*
			//         South

			m_cbTessellation.m_v->size = Vector2(rect.Width(), rect.Height());
			m_cbTessellation.m_v->level = (float)node->level;
			m_cbTessellation.m_v->edgeLevel[0] = (float)node->data.level[eDirection::WEST];
			m_cbTessellation.m_v->edgeLevel[1] = (float)node->data.level[eDirection::SOUTH];
			m_cbTessellation.m_v->edgeLevel[2] = (float)node->data.level[eDirection::EAST];
			m_cbTessellation.m_v->edgeLevel[3] = (float)node->data.level[eDirection::NORTH];
			m_cbTessellation.Update(renderer, 6);

			m_vtxBuff.Bind(renderer);
			renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
			renderer.GetDevContext()->Draw(m_vtxBuff.GetVertexCount(), 0);

		}
		else
		{
			for (int i = 0; i < 4; ++i)
				if (node->children[i])
					g_stack[sp++].node = node->children[i];
		}
	}

	renderer.UnbindShaderAll();
}


void cTerrainQuadTree::RenderQuad(graphic::cRenderer &renderer, const Ray &ray)
{
	int sp = 0;
	g_stack[sp++] = { sRectf(), 0, m_qtree.m_root };

	while (sp > 0)
	{
		sQuadTreeNode<sQuadData> *node = g_stack[sp - 1].node;
		--sp;

		// leaf node?
		if (!node->children[0])
		{
			const sRectf rect = m_qtree.GetNodeRect(node);

			Vector3 lines[] = {
				Vector3(rect.left, 0, rect.top)
				, Vector3(rect.right, 0, rect.top)
				, Vector3(rect.right, 0, rect.bottom)
				, Vector3(rect.left, 0, rect.bottom)
				, Vector3(rect.left, 0, rect.top)
			};

			renderer.m_rect3D.m_color = cColor::BLACK;
			renderer.m_rect3D.SetRect(renderer, lines, ARRAYSIZE(lines));
			renderer.m_rect3D.Render(renderer);

			Plane ground(Vector3(0, 1, 0), 0);
			Vector3 pos = ground.Pick(ray.orig, ray.dir);
			if (sQuadTreeNode<sQuadData> *node = m_qtree.GetNode(sRectf::Rect(pos.x, pos.z, 0, 0)))
			{
				vector<std::pair<sRectf, cColor>> ars;

				const sRectf rect = m_qtree.GetNodeRect(node);
				ars.push_back({ rect, cColor::WHITE });

				if (sQuadTreeNode<sQuadData> *north = m_qtree.GetNorthNeighbor(node))
				{
					const sRectf r = m_qtree.GetNodeRect(north);
					ars.push_back({ r, cColor::RED });
				}

				if (sQuadTreeNode<sQuadData> *east = m_qtree.GetEastNeighbor(node))
				{
					const sRectf r = m_qtree.GetNodeRect(east);
					ars.push_back({ r, cColor::GREEN });
				}

				if (sQuadTreeNode<sQuadData> *south = m_qtree.GetSouthNeighbor(node))
				{
					const sRectf r = m_qtree.GetNodeRect(south);
					ars.push_back({ r, cColor::BLUE });
				}

				if (sQuadTreeNode<sQuadData> *west = m_qtree.GetWestNeighbor(node))
				{
					const sRectf r = m_qtree.GetNodeRect(west);
					ars.push_back({ r, cColor::YELLOW });
				}

				for (auto &data : ars)
				{
					const sRectf &r = data.first;

					Vector3 lines[] = {
						Vector3(r.left, 0, r.top)
						, Vector3(r.right, 0, r.top)
						, Vector3(r.right, 0, r.bottom)
						, Vector3(r.left, 0, r.bottom)
						, Vector3(r.left, 0, r.top)
					};

					CommonStates state(renderer.GetDevice());
					renderer.GetDevContext()->RSSetState(state.CullNone());
					renderer.GetDevContext()->OMSetDepthStencilState(state.DepthNone(), 0);

					renderer.m_rect3D.m_color = data.second;
					renderer.m_rect3D.SetRect(renderer, lines, ARRAYSIZE(lines));
					renderer.m_rect3D.Render(renderer);

					renderer.GetDevContext()->OMSetDepthStencilState(state.DepthDefault(), 0);
					renderer.GetDevContext()->RSSetState(state.CullCounterClockwise());
				}
			}
		}
		else
		{
			for (int i = 0; i < 4; ++i)
				if (node->children[i])
					g_stack[sp++].node = node->children[i];
		}
	}
}


int cTerrainQuadTree::GetLevel(const float distance)
{
	return (int)(distance / 10.f) - 1;
}


// 다른 레벨의 쿼드가 인접해 있을 때, 테셀레이션을 이용해서, 마주보는 엣지의 버텍스 갯수를 맞춰준다.
// When other levels of quads are adjacent, use tessellation to match the number of vertices in the facing edge.
void cTerrainQuadTree::CalcSeamlessLevel()
{
	int sp = 0;
	g_stack[sp++] = { sRectf(), 0, m_qtree.m_root };

	while (sp > 0)
	{
		sQuadTreeNode<sQuadData> *node = g_stack[sp - 1].node;
		--sp;

		// leaf node?
		if (!node->children[0])
		{
			if (sQuadTreeNode<sQuadData> *north = m_qtree.GetNorthNeighbor(node))
				CalcQuadEdgeLevel(node, north, eDirection::NORTH);
			else
				node->data.level[eDirection::NORTH] = node->level;

			if (sQuadTreeNode<sQuadData> *east = m_qtree.GetEastNeighbor(node))
				CalcQuadEdgeLevel(node, east, eDirection::EAST);
			else
				node->data.level[eDirection::EAST] = node->level;

			if (sQuadTreeNode<sQuadData> *south = m_qtree.GetSouthNeighbor(node))
				CalcQuadEdgeLevel(node, south, eDirection::SOUTH);
			else
				node->data.level[eDirection::SOUTH] = node->level;

			if (sQuadTreeNode<sQuadData> *west = m_qtree.GetWestNeighbor(node))
				CalcQuadEdgeLevel(node, west, eDirection::WEST);
			else
				node->data.level[eDirection::WEST] = node->level;
		}
		else
		{
			for (int i = 0; i < 4; ++i)
				if (node->children[i])
					g_stack[sp++].node = node->children[i];
		}
	}
}


// 다른 쿼드와 맞물릴 때, 높은 레벨의 쿼드를 배열에 저장한다.
// 테셀레이션에서, 자신의 레벨과, 엣지의 레벨을 비교해, 테셀레이션 팩터를 계산한다.
// Save high-level quads to the array when they are mingled with other quads.
// In tessellation, compute the tessellation factor by comparing its level with the level of the edge.
void cTerrainQuadTree::CalcQuadEdgeLevel(sQuadTreeNode<sQuadData> *from, sQuadTreeNode<sQuadData> *to
	, const eDirection::Enum dir)
{
	eDirection::Enum oppDir = GetOpposite(dir);

	if (from->data.level[dir] < from->level)
		from->data.level[dir] = from->level;

	if (to->data.level[oppDir] < from->data.level[dir])
		to->data.level[oppDir] = from->data.level[dir];

	if (from->data.level[dir] < to->data.level[oppDir])
		from->data.level[dir] = to->data.level[oppDir];
}


cTerrainQuadTree::eDirection::Enum cTerrainQuadTree::GetOpposite(const eDirection::Enum type)
{
	switch (type)
	{
	case eDirection::NORTH: return eDirection::SOUTH;
	case eDirection::EAST: return eDirection::WEST;
	case eDirection::SOUTH: return eDirection::NORTH;
	case eDirection::WEST: return eDirection::EAST;
	default:
		assert(0);
		return eDirection::SOUTH;
	}
}
