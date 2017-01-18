#pragma once
#include "afxcmn.h"
#include "afxeditbrowsectrl.h"


// CSc2Panel ��ȭ �����Դϴ�.

class CSc2Panel : public CPanelBase
								, public common::iObserver2
{
public:
	CSc2Panel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSc2Panel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SC2 };

	virtual void Update(int type) override;


protected:
	void UpdateModelTree();
	void UpdateAnimationTree(const string &modelFileName="");

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CFileTreeCtrl m_modelFileTree;
	CFileTreeCtrl m_aniFileTree;
	CMFCEditBrowseCtrl m_modelBrowser;
	CMFCEditBrowseCtrl m_animationBrowser;
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreeSc2Files(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeSc2aniFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
