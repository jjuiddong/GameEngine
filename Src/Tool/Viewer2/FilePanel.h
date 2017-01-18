#pragma once
#include "PanelBase.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "FileTreeCtrl.h"


namespace graphic  {
	class cBoneNode;
}


class CFilePanel : public CPanelBase
				, public common::iObserver2
{
public:
	CFilePanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFilePanel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FILE };

	virtual void Update(int type) override;
	
	void UpdateModelFiles();
	void UpdateAnimationFiles();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CString m_textModelFile;
	CString m_textAnimationFiles;
	afx_msg void OnBnClickedButtonRefresh();
	CFileTreeCtrl m_modelTree;
	afx_msg void OnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult);
	CFileTreeCtrl m_animationTree;
	afx_msg void OnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult);
};
