#pragma once
#include "afxcmn.h"


// CMainPanelTab 대화 상자입니다.
class CModelPanel;
class CAnimationPanel;
class CFilePanel;
class CArchebladePanel;
class CTeraPanel;
class CLightPanel;
class CMainPanelTab : public CDialogEx
{
public:
	CMainPanelTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMainPanelTab();
	enum { IDD = IDD_DIALOG_MAINPANEL };


protected:
	void ShowPanel(int idx);
	vector<CWnd*> m_panels;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CTabCtrl m_Tab;
};
