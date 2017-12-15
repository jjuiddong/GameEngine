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
	float level;
	float edgeLevel[4]; // north-east-south-west
	//float dummy;
};


class cTerrainQuadTree
{
public:
	struct eDirection { enum Enum { NORTH, EAST, SOUTH, WEST }; };

	struct sQuadData
	{
		int level[4]; // eDirection index

		sQuadData::sQuadData() {
			ZeroMemory(level, sizeof(level));
		}
	};


public:
	cTerrainQuadTree();
	cTerrainQuadTree(sRectf &rect);
	virtual ~cTerrainQuadTree();

	bool Create(graphic::cRenderer &renderer);

	void Render(graphic::cRenderer &renderer, const graphic::cFrustum &frustum
		, const int limitLevel, const int level
		, const Ray &ray);


protected:
	int GetLevel(const float distance);
	void CalcSeamlessLevel();
	void CalcQuadEdgeLevel(sQuadTreeNode<sQuadData> *from, sQuadTreeNode<sQuadData> *to
		, const eDirection::Enum type);
	eDirection::Enum GetOpposite(const eDirection::Enum type);
	void RenderTessellation(graphic::cRenderer &renderer);
	void RenderQuad(graphic::cRenderer &renderer, const Ray &ray);
	void BuildQuadTree(const graphic::cFrustum &frustum);


public:
	bool m_isShowQuadTree;
	sRectf m_rect;
	cQuadTree<sQuadData> m_qtree;
	graphic::cVertexBuffer m_vtxBuff;
	graphic::cShader11 m_shader;
	graphic::cConstantBuffer<sTessellationBuffer> m_cbTessellation;
};
