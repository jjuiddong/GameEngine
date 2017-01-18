#pragma once
#include "afxcmn.h"


// CTabPanel 대화 상자입니다.
class CHeightMapPanel;
class CTerrainPanel;
class CBrushPanel;
class CModelPanel;

class CTabPanel : public CDialogEx
{
public:
	CTabPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTabPanel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TABPANEL };


protected:
	void ShowPanel(int idx);
	vector<CWnd*> m_panels;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CTabCtrl m_Tab;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
