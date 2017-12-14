
#include "stdafx.h"
#include "terrainquadtree.h"

using namespace graphic;

// Quad Tree Traverse Stack Memory (no multithread safe!!)
struct sData
{
	sQuadTreeNode *node;
};
sData g_stack[1024];


cTerrainQuadTree::cTerrainQuadTree()
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

	sQuadTreeNode *qnode = new sQuadTreeNode;
	m_qtree.Insert(qnode);
	m_qtree.InsertChildren(qnode);
	m_qtree.InsertChildren(qnode->children[0]);
	m_qtree.InsertChildren(qnode->children[0]->children[0]);
	m_qtree.InsertChildren(qnode->children[1]);
	m_qtree.InsertChildren(qnode->children[1]->children[0]);
	m_qtree.InsertChildren(qnode->children[1]->children[1]);
	m_qtree.InsertChildren(qnode->children[1]->children[1]->children[1]);

	return true;
}


void cTerrainQuadTree::Render(graphic::cRenderer &renderer
	, const graphic::cFrustum &frustum
	, const int limitLevel, const int level
	, const Ray &ray 
)
{

	int sp = 0;
	g_stack[sp++] = { m_qtree.m_root };

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

