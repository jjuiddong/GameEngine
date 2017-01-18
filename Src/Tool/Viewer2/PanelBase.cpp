// PanelBase.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "PanelBase.h"


// CPanelBase 대화 상자입니다.

CPanelBase::CPanelBase(UINT id, CWnd* pParent /*=NULL*/)
	: CDialogEx(id, pParent)
{

}

CPanelBase::~CPanelBase()
{
}

void CPanelBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelBase, CDialogEx)
END_MESSAGE_MAP()


// CPanelBase 메시지 처리기입니다.


// 모든 트리노드를 펼친다.
void CPanelBase::ExpandAll(CTreeCtrl &treeCtrl)
{
	HTREEITEM hRoot = treeCtrl.GetRootItem();
	vector<HTREEITEM> items;
	items.reserve(treeCtrl.GetCount());

	items.push_back(hRoot);

	while (!items.empty())
	{
		HTREEITEM hItem = items.back();
		items.pop_back();
		treeCtrl.Expand(hItem, TVE_EXPAND);

		HTREEITEM hfirstChild = treeCtrl.GetChildItem(hItem);
		if (hfirstChild)
		{
			items.push_back(hfirstChild);

			while (HTREEITEM hNextItem = treeCtrl.GetNextSiblingItem(hfirstChild))
			{
				items.push_back(hNextItem);
				hfirstChild = hNextItem;
			}
		}
	}

	treeCtrl.SelectSetFirstVisible(hRoot);
}


void CPanelBase::MoveChildCtrlWindow(CWnd &wndCtrl, int cx, int cy)
{
	if (wndCtrl.GetSafeHwnd())
	{
		CRect wr;
		wndCtrl.GetWindowRect(wr);
		ScreenToClient(wr);
		wndCtrl.MoveWindow(wr.left, wr.top, cx, wr.Height());
	}
}


HTREEITEM CPanelBase::FindTree( CTreeCtrl &treeCtrl, const wstring &findText )
{
	HTREEITEM hRoot = treeCtrl.GetRootItem();
	vector<HTREEITEM> items;
	items.reserve(treeCtrl.GetCount());

	wstring lowerText = findText;
	lowerCasew(lowerText);

	items.push_back(hRoot);

	while (!items.empty())
	{
		HTREEITEM hItem = items.back();
		items.pop_back();
		CString itemText = treeCtrl.GetItemText(hItem);
		CString lowerItemText = lowerCasew((wstring)itemText).c_str();

		const int idx = lowerItemText.Find(lowerText.c_str());
		if (idx >= 0)
			return hItem;

		HTREEITEM hfirstChild = treeCtrl.GetChildItem(hItem);
		if (hfirstChild)
		{
			items.push_back(hfirstChild);

			while (HTREEITEM hNextItem = treeCtrl.GetNextSiblingItem(hfirstChild))
			{
				items.push_back(hNextItem);
				hfirstChild = hNextItem;
			}
		}
	}

	return NULL;
}
