
#include "stdafx.h"
#include "terrainquadtree.h"

using namespace graphic;

// Quad Tree Traverse Stack Memory (no multithread safe!!)
struct sData
{
	sRectf rect;
	int level;
	sQuadTreeNode *node;
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

	if (!m_shader.Create(renderer, "../media/shader11/tess-pos.fxo", "Unlit"
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
	m_qtree.Clear();
	m_qtree.Insert(new sQuadTreeNode);

	int sp = 0;
	g_stack[sp++] = { m_rect, level, m_qtree.m_root };

	m_shader.SetTechnique("Unlit");
	m_shader.Begin();
	m_shader.BeginPass(renderer, 0);

	while (sp > 0)
	{
		const sRectf rect = g_stack[sp - 1].rect;
		const int lv = g_stack[sp - 1].level;
		sQuadTreeNode *parentNode = g_stack[sp - 1].node;
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
		const int curLevel = GetLevel(distance);// (int)(distance / 10.f) - 1;
		//const int offsetLevel = (int)(radius / 10.f) + 1;
		const bool isMinSize = hw < 25.f;

		//if (isMinSize || (lv <= (curLevel - offsetLevel)))
		if (isMinSize || (lv <= curLevel))
		{
			if (!m_isShowQuadTree)
			{
				Transform tfm;
				tfm.pos.x = rect.left;
				tfm.pos.z = rect.top;
				tfm.pos.y = 0.f;
				renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM());
				renderer.m_cbPerFrame.Update(renderer);

				m_cbTessellation.m_v->size = Vector2(rect.Width(), rect.Height());
				m_cbTessellation.Update(renderer, 6);

				m_vtxBuff.Bind(renderer);
				renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
				renderer.GetDevContext()->Draw(m_vtxBuff.GetVertexCount(), 0);
			}

			continue;
		}

		m_qtree.InsertChildren(parentNode);

		g_stack[sp++] = { sRectf::Rect(rect.left, rect.top, hw, hh), lv - 1, parentNode->children[0] };
		g_stack[sp++] = { sRectf::Rect(rect.left + hw, rect.top, hw, hh), lv - 1, parentNode->children[1] };
		g_stack[sp++] = { sRectf::Rect(rect.left, rect.top + hh, hw, hh), lv - 1, parentNode->children[2] };
		g_stack[sp++] = { sRectf::Rect(rect.left + hw, rect.top + hh, hw, hh), lv - 1, parentNode->children[3] };

		assert(sp < 1024);
	}

	renderer.UnbindShaderAll();


	if (m_isShowQuadTree)
	{
		sp = 0;
		g_stack[sp++] = { m_rect, level, m_qtree.m_root };

		while (sp > 0)
		{
			sQuadTreeNode *node = g_stack[sp - 1].node;
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
			}

			for (int i = 0; i < 4; ++i)
				if (node->children[i])
					g_stack[sp++].node = node->children[i];
		}

		Plane ground(Vector3(0, 1, 0), 0);
		Vector3 pos = ground.Pick(ray.orig, ray.dir);
		if (sQuadTreeNode *node = m_qtree.GetNode(sRectf::Rect(pos.x, pos.z, 0, 0)))
		{
			vector<std::pair<sRectf, cColor>> ars;

			const sRectf rect = m_qtree.GetNodeRect(node);
			ars.push_back({ rect, cColor::WHITE });

			if (sQuadTreeNode *north = m_qtree.GetNorthNeighbor(node))
			{
				const sRectf r = m_qtree.GetNodeRect(north);
				ars.push_back({ r, cColor::RED });
			}

			if (sQuadTreeNode *east = m_qtree.GetEastNeighbor(node))
			{
				const sRectf r = m_qtree.GetNodeRect(east);
				ars.push_back({ r, cColor::GREEN });
			}

			if (sQuadTreeNode *south = m_qtree.GetSouthNeighbor(node))
			{
				const sRectf r = m_qtree.GetNodeRect(south);
				ars.push_back({ r, cColor::BLUE });
			}

			if (sQuadTreeNode *west = m_qtree.GetWestNeighbor(node))
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

}


int cTerrainQuadTree::GetLevel(const float distance)
{
	return (int)(distance / 10.f) - 1;

	//const float DISTANCE = 2500.f;

	//if (distance >= DISTANCE)
	//	return 15;
	//
	//float d = DISTANCE;
	//float r = DISTANCE;

	//int lv = 14;
	//do
	//{
	//	r *= 0.5f;
	//	d = DISTANCE - r;
	//	if (distance >= d)
	//		return lv;

	//} while ((lv > 0) && (d > 25.f));

	//return 0;
}
