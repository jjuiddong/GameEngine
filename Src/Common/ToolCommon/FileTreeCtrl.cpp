// FileTreeCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FileTreeCtrl.h"
#include "UIUtiltity.h"


CImageList *CFileTreeCtrl::m_imageList = NULL;
int CFileTreeCtrl::m_imageListRefCnt = 0;

// CFileTreeCtrl
CFileTreeCtrl::CFileTreeCtrl()
{
	if (!m_imageList)
	{
		SHFILEINFO sfi = { 0 }; 
		m_imageList = new CImageList();
		m_imageList->Attach((HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX));
	}

	++m_imageListRefCnt;

}

CFileTreeCtrl::~CFileTreeCtrl()
{
	--m_imageListRefCnt;
	if (m_imageListRefCnt <= 0)
	{
		SAFE_DELETE(m_imageList);
		m_imageListRefCnt = 0;
	}
}


BEGIN_MESSAGE_MAP(CFileTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()



// CFileTreeCtrl �޽��� ó�����Դϴ�.

// fileList�� ����� ���� ��� ��Ʈ������ Ʈ���� �����.
void CFileTreeCtrl::Update(const list<string> &fileList)
{
	DeleteAllItems();	

	SetImageList(m_imageList, TVSIL_NORMAL);

	sTreeNode *rootNode = GenerateTreeNode(fileList);
	GenerateTreeItem(NULL, rootNode);	
	Expand(GetRootItem(), TVE_EXPAND);

	DeleteTreeNode(rootNode);

	m_fileCount = (int)fileList.size();
}


void CFileTreeCtrl::Update(const string &directoryPath, const list<string> &extList)
{
	list<string> files;
	common::CollectFiles(extList, directoryPath, files);
	Update(files);
}


// searchStr ���ڿ��� ���Ե� �͸� ȭ�鿡 ����Ѵ�.
void CFileTreeCtrl::Update(const string &directoryPath, const list<string> &extList, const string &serchStr )
{
	list<string> files;
	common::CollectFiles(extList, directoryPath, files);

	if (serchStr.empty())
	{
		Update(files);
	}
	else
	{
		list<string> fileList;
		BOOST_FOREACH (string &name, files)
		{
			if (string::npos != common::GetFileName(name).find(serchStr))
				fileList.push_back(name);
		}

		Update(fileList);
	}	
}


// sTreeNode �����Ѵ�.
CFileTreeCtrl::sTreeNode* CFileTreeCtrl::GenerateTreeNode(const list<string> &fileList)
{
	sTreeNode *rootNode = new sTreeNode;

	BOOST_FOREACH (auto &str, fileList)
	{
		vector<string> strs;
		common::tokenizer( str, "/", ".", strs);

		sTreeNode *node = rootNode;
		BOOST_FOREACH (auto &name, strs)
		{
			auto it = node->children.find(name);
			if (node->children.end() ==  it)
			{
				sTreeNode *t = new sTreeNode;
				node->children[ name] = t;
				node = t;
			}
			else
			{
				node = it->second;
			}
		}	
	}

	return rootNode;
}


// treenode ����.
void CFileTreeCtrl::DeleteTreeNode(sTreeNode *node)
{
	RET(!node);
	BOOST_FOREACH (auto &child, node->children)
		DeleteTreeNode(child.second);
	delete node;
}


// sTreeNode ������ TreeCtrl ��带 �����Ѵ�.
void CFileTreeCtrl::GenerateTreeItem(HTREEITEM parent, sTreeNode*node)
{
	RET(!node);

	BOOST_FOREACH (auto &child, node->children)
	{
		const int imageIdx = child.second->children.empty()? 0 : 1;
		HTREEITEM item = InsertItem(str2wstr(child.first).c_str(), imageIdx, imageIdx, parent);
		GenerateTreeItem(item, child.second);
	}
}


// ���õ� Ʈ������� ���� ��θ� �����Ѵ�.
string CFileTreeCtrl::GetSelectFilePath(HTREEITEM item)
{
	wstring path;
	HTREEITEM curItem = item;
	while (curItem)
	{
		if (path.empty())
			path = (wstring)GetItemText(curItem);
		else
			path = (wstring)GetItemText(curItem) + L"/" + path;

		curItem = GetParentItem(curItem);
	}

	return wstr2str(path);
}


// ��� Ʈ�� ������ ��ģ��.
void CFileTreeCtrl::ExpandAll()
{
	::ExpandAll(*this);
}
