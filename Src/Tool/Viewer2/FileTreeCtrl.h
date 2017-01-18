#pragma once


// CFileTreeCtrl

class CFileTreeCtrl : public CTreeCtrl
{
public:
	CFileTreeCtrl();
	virtual ~CFileTreeCtrl();

	void Update(const list<string> &fileList);
	void Update(const string &directoryPath, const list<string> &extList);
	void Update(const string &directoryPath, const list<string> &extList, const string &serchStr );
	string GetSelectFilePath(HTREEITEM item);
	int GetFileCount() const;
	void ExpandAll();


protected:
	struct sTreeNode 
	{
		map<string, sTreeNode*> children;
	};

	sTreeNode* GenerateTreeNode(const list<string> &fileList);
	void DeleteTreeNode(sTreeNode *node);
	void GenerateTreeItem(HTREEITEM parent, sTreeNode*node);

	DECLARE_MESSAGE_MAP()

	static CImageList *m_imageList;
	static int m_imageListRefCnt;
	int m_fileCount;
};


inline int CFileTreeCtrl::GetFileCount() const { return m_fileCount; }
