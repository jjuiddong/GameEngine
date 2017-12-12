//
// 2017-12-12, jjuiddong
// QuadTree for Terrain
// (no multithread safe!!)
//
#pragma once


class cQuadTree
{
public:
	cQuadTree();
	cQuadTree(sRectf &rect);
	virtual ~cQuadTree();

	void Render(graphic::cRenderer &renderer, const graphic::cFrustum &frustum
		, const int limitLevel, const int level);


public:
	sRectf m_rect;
};
