// ���� ���� ����ϴ� Ŭ������.
// ���콺 ������ ��ư�� ������ �޴� â�� ��� �� �ִ�.
// �˻� ���, Ʈ�� ��ġ�� ���, ���õ� ��带 ����ϴ� ����� �ִ�.
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

