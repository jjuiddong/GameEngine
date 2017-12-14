//
// 2017-12-14, jjuiddong
// Quad-Tree (Sparse QuadTree)
//
//- Game Programming Gems2, 4.5
//
//- Simple and Efficient Traversal Methods for Quadtrees and Octrees
//       Sarah F. Frisken
//       Ronald N.Perry
//       TR2002 - 41 November 2002
//
#pragma once


struct sQuadTreeNode
{
	int xLoc;
	int yLoc;
	int level;
	sQuadTreeNode *parent; // reference
	sQuadTreeNode *children[4]; // reference

	sQuadTreeNode() {
		xLoc = 0;
		yLoc = 0;
		level = 0;
		parent = NULL;
		children[0] = children[1] = children[2] = children[3] = NULL;
	}
};


class cQuadTree
{
public:
	cQuadTree();
	~cQuadTree();

	bool Insert(sQuadTreeNode *node);
	bool Remove(sQuadTreeNode *node, const bool isRmTree = true);
	bool InsertChildren(sQuadTreeNode *node);
	bool RemoveChildren(sQuadTreeNode *node, const bool isRmTree = true);
	sQuadTreeNode* GetNode(const sRectf &rect);
	sQuadTreeNode* GetNode(const int level, const int xLoc, const int yLoc);
	sQuadTreeNode* GetNorthNeighbor(sQuadTreeNode *node);
	sQuadTreeNode* GetSouthNeighbor(sQuadTreeNode *node);
	sQuadTreeNode* GetWestNeighbor(sQuadTreeNode *node);
	sQuadTreeNode* GetEastNeighbor(sQuadTreeNode *node);
	sRectf GetNodeRect(sQuadTreeNode *node);
	void Clear();


public:
	enum {MAX_LEVEL = 8};

	sQuadTreeNode *m_root;
	const float m_quadScale = 1.f;
	std::map<int, sQuadTreeNode*> m_nodeTable[MAX_LEVEL]; // level quad tree map
														  // key = quad node x,y index (linear)
};
