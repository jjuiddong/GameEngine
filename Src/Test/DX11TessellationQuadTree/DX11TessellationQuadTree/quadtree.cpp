
#include "stdafx.h"
#include "quadtree.h"

using namespace graphic;

// Quad Tree Traverse Stack Memory (no multithread safe!!)
struct sData
{
	sRectf rect;
	int level;
};
sData g_stack[1024];


cQuadTree::cQuadTree()
{
}

cQuadTree::cQuadTree(sRectf &rect)
	: m_rect(rect)
{
}

cQuadTree::~cQuadTree()
{
}


void cQuadTree::Render(graphic::cRenderer &renderer
	, const graphic::cFrustum &frustum
	, const int limitLevel, const int level)
{
	int sp = 0;
	g_stack[sp++] = { m_rect, level };

	while (sp > 0)
	{
		const sRectf rect = g_stack[sp - 1].rect;
		const int lv = g_stack[sp - 1].level;
		--sp;

		const float hw = rect.Width() / 2.f;
		const float hh = rect.Height() / 2.f;
		const Vector3 center = Vector3(rect.Center().x, 0, rect.Center().y);
		const float radius = sqrt(hw*hw + hh*hh);

		cBoundingSphere bsphere;
		bsphere.SetBoundingSphere(center, radius);
		if (!frustum.IsInSphere(bsphere))
			continue;

		const float distance = center.Distance(frustum.m_pos);
		const int curLevel = (int)(distance / 10.f);
		const int offsetLevel = (int)(radius / 10.f) + 1;
		const bool isMinSize = hw < 50.f;

		if (isMinSize || (lv <= (curLevel - offsetLevel)))
		{
			// render
			Vector3 lines[] = {
				Vector3(rect.left, 0, rect.top)
				, Vector3(rect.right, 0, rect.top)
				, Vector3(rect.right, 0, rect.bottom)
				, Vector3(rect.left, 0, rect.bottom)
				, Vector3(rect.left, 0, rect.top)
			};
			renderer.m_rect3D.SetRect(renderer, lines, 5);
			renderer.m_rect3D.Render(renderer);
			continue;
		}

		g_stack[sp++] = { sRectf::Rect(rect.left, rect.top, hw, hh), lv - 1 };
		g_stack[sp++] = { sRectf::Rect(rect.left + hw, rect.top, hw, hh), lv - 1 };
		g_stack[sp++] = { sRectf::Rect(rect.left, rect.top + hh, hw, hh), lv - 1 };
		g_stack[sp++] = { sRectf::Rect(rect.left + hw, rect.top + hh, hw, hh), lv - 1 };

		assert(sp < 1024);
	}
}

