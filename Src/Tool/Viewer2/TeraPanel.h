#pragma once
#include "afxeditbrowsectrl.h"
#include "afxcmn.h"


// CTeraPanel ��ȭ �����Դϴ�.

class CTeraPanel : public CDialogEx
							, public common::iObserver2
{
public:
	CTeraPanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTeraPanel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TERA };

	virtual void Update(int type) override;


protected:
	int m_selectModelType;
	void UpdateModelTree();
	void UpdateAnimationTree();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CMFCEditBrowseCtrl m_bodyBrowser;
	CMFCEditBrowseCtrl m_handBrowser;
	CMFCEditBrowseCtrl m_legBrowser;
	CMFCEditBrowseCtrl m_faceBrowser;
	CMFCEditBrowseCtrl m_hairBrowser;
	CMFCEditBrowseCtrl m_tailBrowser;
	CFileTreeCtrl m_fileTree;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioBody();
	afx_msg void OnBnClickedRadioHand();
	afx_msg void OnBnClickedRadioLeg();
	afx_msg void OnBnClickedRadioFace();
	afx_msg void OnBnClickedRadioHair();
	afx_msg void OnBnClickedRadioTail();
	afx_msg void OnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult);
	CFileTreeCtrl m_animationTree;
	afx_msg void OnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRefresh();
};
