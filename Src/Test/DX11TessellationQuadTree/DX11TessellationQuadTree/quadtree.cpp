
#include "stdafx.h"
#include "quadtree.h"


// m_nodeTable key
// Level            Key
//   1               0
//   2       0       |        1       |       2      |        3
//   3    0,1,2,3    |    4,5,6,7     |  8,9,10,11   |   12,13,14,15
//   4    ....

cQuadTree::cQuadTree()
	: m_root(NULL)
{
}

cQuadTree::~cQuadTree()
{
	Clear();
}


inline int MakeKey(const int level, const int xLoc, const int yLoc)
{
	return (yLoc << (level)) + xLoc;
}


bool cQuadTree::Insert(sQuadTreeNode *node)
{
	RETV(node->level >= MAX_LEVEL, false);

	const int key = MakeKey(node->level, node->xLoc, node->yLoc);

	auto it = m_nodeTable[node->level].find(key);
	if (m_nodeTable[node->level].end() != it)
		return false; // Error!! Already Exist

	m_nodeTable[node->level][key] = node;

	if (node->level == 0)
		m_root = node;

	return true;
}


bool cQuadTree::Remove(sQuadTreeNode *node
	, const bool isRmTree //= true
)
{
	RETV(node->level >= MAX_LEVEL, false);

	const int key = MakeKey(node->level, node->xLoc, node->yLoc);

	auto it = m_nodeTable[node->level].find(key);
	if (m_nodeTable[node->level].end() == it)
		return false; // Error!! Not Exist

	for (int i = 0; i < 4; ++i)
		if (node->children[i])
			Remove(node->children[i], isRmTree);

	m_nodeTable[node->level].erase(key);

	if (isRmTree)
		delete node;

	return true;
}


// node의 chilren에 node를 추가한다.
bool cQuadTree::InsertChildren(sQuadTreeNode *node)
{
	RETV((node->level+1) >= MAX_LEVEL, false);

	int locs[] = { 0,0, 1,0, 0,1, 1,1 }; //x,y loc

	for (int i = 0; i < 4; ++i)
	{
		sQuadTreeNode *p = new sQuadTreeNode;
		p->xLoc = (node->xLoc << node->level) + locs[i * 2];
		p->yLoc = (node->yLoc << node->level) + locs[i * 2 + 1];
		p->level = node->level + 1;
		p->parent = node;
		node->children[i] = p;
		Insert(p);
	}

	return true;
}


bool cQuadTree::RemoveChildren(sQuadTreeNode *node
	, const bool isRmTree //= true
)
{
	for (int i = 0; i < 4; ++i)
	{
		if (node->children[i])
		{
			Remove(node->children[i], isRmTree);
			node->children[i] = NULL;
		}
	}
	return true;
}


sQuadTreeNode* cQuadTree::GetNode(const sRectf &rect)
{
	int x1 = (int)(rect.left * m_quadScale);
	int y1 = (int)(rect.top * m_quadScale);

	int xResult = x1 ^ ((int)(rect.right * m_quadScale));
	int yResult = y1 ^ ((int)(rect.bottom * m_quadScale));

	int nodeLevel = MAX_LEVEL;
	int shiftCount = 0;

	while (xResult + yResult != 0)
	{
		xResult >>= 1;
		yResult >>= 1;
		nodeLevel--;
		shiftCount++;
	}

	x1 >>= shiftCount;
	y1 >>= shiftCount;

	sQuadTreeNode *ret = NULL;
	do
	{
		ret = GetNode(nodeLevel, x1, y1);
		if (!ret && (nodeLevel <= 0))
			break;

		// goto parent level
		x1 >>= 1;
		y1 >>= 1;
		--nodeLevel;
	} while (ret == NULL);

	return ret;
}


sQuadTreeNode* cQuadTree::GetNode(const int level, const int xLoc, const int yLoc)
{
	if (level >= MAX_LEVEL)
		return NULL;

	const int key = MakeKey(level, xLoc, yLoc);

	auto it = m_nodeTable[level].find(key);
	if (m_nodeTable[level].end() == it)
		return false; // Error!! Not Exist

	return m_nodeTable[level][key];
}


sQuadTreeNode* cQuadTree::GetNorthNeighbor(sQuadTreeNode *node)
{
	sRectf rect = GetNodeRect(node);
	Vector2 pos = rect.Center();
	pos.y += (rect.Height() / 2.f + (m_quadScale / 2.f));

	sQuadTreeNode *ret = GetNode(sRectf::Rect(pos.x, pos.y, 0, 0));
	return (ret == node) ? NULL : ret;
}


sQuadTreeNode* cQuadTree::GetSouthNeighbor(sQuadTreeNode *node)
{
	sRectf rect = GetNodeRect(node);
	Vector2 pos = rect.Center();
	pos.y -= (rect.Height() / 2.f + (m_quadScale / 2.f));

	sQuadTreeNode *ret = GetNode(sRectf::Rect(pos.x, pos.y, 0, 0));
	return (ret == node) ? NULL : ret;
}


sQuadTreeNode* cQuadTree::GetWestNeighbor(sQuadTreeNode *node)
{
	sRectf rect = GetNodeRect(node);
	Vector2 pos = rect.Center();
	pos.x -= (rect.Width() / 2.f + (m_quadScale / 2.f));

	sQuadTreeNode *ret = GetNode(sRectf::Rect(pos.x, pos.y, 0, 0));
	return (ret == node) ? NULL : ret;
}


sQuadTreeNode* cQuadTree::GetEastNeighbor(sQuadTreeNode *node)
{
	sRectf rect = GetNodeRect(node);
	Vector2 pos = rect.Center();
	pos.x += (rect.Width() / 2.f + (m_quadScale / 2.f));

	sQuadTreeNode *ret = GetNode(sRectf::Rect(pos.x, pos.y, 0, 0));
	return (ret == node) ? NULL : ret;
}


sRectf cQuadTree::GetNodeRect(sQuadTreeNode *node)
{
	const float size = (float)(1 << (MAX_LEVEL - node->level)) * m_quadScale;
	const sRectf rect = sRectf::Rect(node->xLoc * size, node->yLoc*size, size, size);
	return rect;
}


void cQuadTree::Clear()
{
	for (int i = 0; i < MAX_LEVEL; ++i)
	{
		for (auto kv : m_nodeTable[i])
			delete kv.second;
		m_nodeTable[i].clear();
	}
	m_root = NULL;
}

