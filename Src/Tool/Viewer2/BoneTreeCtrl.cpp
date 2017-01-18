// BoneTreeCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "BoneTreeCtrl.h"
#include "inputdlg.h"
#include "UIUtiltity.h"


// CBoneTreeCtrl
CBoneTreeCtrl::CBoneTreeCtrl()
{

}

CBoneTreeCtrl::~CBoneTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CBoneTreeCtrl, CTreeCtrl)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_RCLICK, &CBoneTreeCtrl::OnNMRClick)
	ON_COMMAND(ID_BONETREE_TREESEARCH, &CBoneTreeCtrl::OnBonetreeTreesearch)
	ON_NOTIFY_REFLECT_EX(TVN_SELCHANGED, &CBoneTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()



// CBoneTreeCtrl 메시지 처리기입니다.
bool CBoneTreeCtrl::Update(graphic::cBoneMgr *boneMgr)
{
	RETV(!boneMgr, false);

	DeleteAllItems();

	const wstring rootStr = formatw( "Bones Tree");
	const HTREEITEM hRoot = InsertItem(rootStr.c_str());
	MakeBoneTree(hRoot, boneMgr->GetRoot());

	return true;
}


// 본트리를 생성한다.
void CBoneTreeCtrl::MakeBoneTree(const HTREEITEM hParent,  graphic::cBoneNode *node)
{
	//const wstring nodeStr = formatw( "%s [%d]", node->GetName().c_str(), node->GetId() );
	const wstring nodeStr = formatw( "%s", node->GetName().c_str() );
	const HTREEITEM hItem = InsertItem(nodeStr.c_str(), hParent);

	BOOST_FOREACH (auto &child, node->GetChildren())
	{
		MakeBoneTree(hItem, (graphic::cBoneNode*)child);
	}
}


// 모든 트리 노드를 확장한다.
void CBoneTreeCtrl::ExpandAll()
{
	::ExpandAll(*this);
}


// 트리 노드 중에 findText 와 같은 문자열이 있다면 찾아서 리턴한다.
// 대소문자 구분하지 않고 검색한다.
HTREEITEM CBoneTreeCtrl::FindTree( const wstring &findText )
{
	HTREEITEM hRoot = GetRootItem();
	vector<HTREEITEM> items;
	items.reserve(GetCount());

	wstring lowerText = findText;
	lowerCasew(lowerText);

	items.push_back(hRoot);

	while (!items.empty())
	{
		HTREEITEM hItem = items.back();
		items.pop_back();
		CString itemText = GetItemText(hItem);
		CString lowerItemText = lowerCasew((wstring)itemText).c_str();

		const int idx = lowerItemText.Find(lowerText.c_str());
		if (idx >= 0)
			return hItem;

		HTREEITEM hfirstChild = GetChildItem(hItem);
		if (hfirstChild)
		{
			items.push_back(hfirstChild);

			while (HTREEITEM hNextItem = GetNextSiblingItem(hfirstChild))
			{
				items.push_back(hNextItem);
				hfirstChild = hNextItem;
			}
		}
	}

	return NULL;
}


void CBoneTreeCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint p;
	GetCursorPos(&p);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_BONETREE_TREESEARCH, _T("Bone Search"));
	menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);	

	*pResult = 0;
}


void CBoneTreeCtrl::OnBonetreeTreesearch()
{
	CInputDlg dialog;
	dialog.m_label = TEXT("Enter a name:");
	if (dialog.DoModal() == IDOK) 
	{
		const wstring str = dialog.m_value;
		const HTREEITEM hItem = FindTree(str);
		if (hItem)
		{
			SelectSetFirstVisible(hItem);
			SelectItem(hItem);
			SetFocus();
		}
		else
		{
			AfxMessageBox(L"Not Found");
		}
	}
}



BOOL CBoneTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	// 선택한 노드의 본을 강조시킨다.
	CString str = GetItemText(pNMTreeView->itemNew.hItem);
	cController::Get()->GetCharacterAnalyzer()->HighlightBone( wstr2str((wstring)str) );

	// http://stackoverflow.com/questions/627143/how-to-handle-notify-messages-in-child-parent-classes
	return FALSE; // 부모에게 이벤트를 보내려면 false를 리턴해야 한다.
}
