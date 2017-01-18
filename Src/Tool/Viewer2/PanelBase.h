#pragma once


// CPanelBase 대화 상자입니다.

class CPanelBase : public CDialogEx
{
public:
	CPanelBase(UINT id, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPanelBase();


protected:
	void ExpandAll(CTreeCtrl &treeCtrl);
	HTREEITEM FindTree( CTreeCtrl &treeCtrl, const wstring &text );
	void MoveChildCtrlWindow(CWnd &wndCtrl, int cx, int cy);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
