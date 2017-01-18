
#include "stdafx.h"
#include "UIUtiltity.h"



// 모든 트리노드를 펼친다.
void ExpandAll(CTreeCtrl &treeCtrl)
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


HTREEITEM FindTree( CTreeCtrl &treeCtrl, const wstring &findText )
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


void MoveChildCtrlWindow(CWnd &parent, CWnd &wndCtrl, int cx, int cy)
{
	if (wndCtrl.GetSafeHwnd())
	{
		CRect wr;
		wndCtrl.GetWindowRect(wr);
		parent.ScreenToClient(wr);
		wndCtrl.MoveWindow(wr.left, wr.top, cx, wr.Height());
	}
}
