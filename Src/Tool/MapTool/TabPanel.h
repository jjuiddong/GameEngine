#pragma once
#include "afxcmn.h"


// CTabPanel ��ȭ �����Դϴ�.
class CHeightMapPanel;
class CTerrainPanel;
class CBrushPanel;
class CModelPanel;

class CTabPanel : public CDialogEx
{
public:
	CTabPanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTabPanel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TABPANEL };


protected:
	void ShowPanel(int idx);
	vector<CWnd*> m_panels;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CTabCtrl m_Tab;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
