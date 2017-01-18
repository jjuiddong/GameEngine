#pragma once
#include "afxcmn.h"
#include "afxeditbrowsectrl.h"


// CSc2Panel 대화 상자입니다.

class CSc2Panel : public CPanelBase
								, public common::iObserver2
{
public:
	CSc2Panel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSc2Panel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SC2 };

	virtual void Update(int type) override;


protected:
	void UpdateModelTree();
	void UpdateAnimationTree(const string &modelFileName="");

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
