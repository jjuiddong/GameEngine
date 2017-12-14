//
// 2017-12-12, jjuiddong
// QuadTree for Terrain
// (no multithread safe!!)
//
#pragma once

#include "quadtree.h"


struct sTessellationBuffer
{
	float tessellationAmount;
	Vector2 size;
	float dummy;
};


class cTerrainQuadTree
{
public:
	cTerrainQuadTree();
	cTerrainQuadTree(sRectf &rect);
	virtual ~cTerrainQuadTree();

	bool Create(graphic::cRenderer &renderer);

	void Render(graphic::cRenderer &renderer, const graphic::cFrustum &frustum
		, const int limitLevel, const int level
		, const Ray &ray);


public:
	sRectf m_rect;
	cQuadTree m_qtree;
	graphic::cVertexBuffer m_vtxBuff;
	graphic::cShader11 m_shader;
	graphic::cConstantBuffer<sTessellationBuffer> m_cbTessellation;
};
