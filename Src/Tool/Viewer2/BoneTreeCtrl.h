// 모델의 본을 출력하는 클래스다.
// 마우스 오른쪽 버튼을 눌러서 메뉴 창을 띄울 수 있다.
// 검색 기능, 트리 펼치기 기능, 선택된 노드를 출력하는 기능이 있다.
// 
#pragma once


// CBoneTreeCtrl

class CBoneTreeCtrl : public CTreeCtrl
{
public:
	CBoneTreeCtrl();
	virtual ~CBoneTreeCtrl();

	bool Update(graphic::cBoneMgr *boneMgr);
	HTREEITEM FindTree( const wstring &findText );
	void ExpandAll();


protected:
	void MakeBoneTree(const HTREEITEM hParent,  graphic::cBoneNode *node);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBonetreeTreesearch();
	afx_msg BOOL OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};

