#pragma once
#include "afxcmn.h"
#include "afxeditbrowsectrl.h"
#include "FileTreeCtrl.h"

// CArchebladePanel ��ȭ �����Դϴ�.

class CArchebladePanel : public CDialogEx
									, public common::iObserver2
{
public:
	CArchebladePanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CArchebladePanel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ARCHEBLADE };

	virtual void Update(int type) override;


protected:
	void UpdateModelTree();
	void UpdateWeaponTree();
	void UpdateAnimationTree();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CFileTreeCtrl m_modelTree;
	CFileTreeCtrl m_weaponTree;
	CFileTreeCtrl m_animationTree;
	CMFCEditBrowseCtrl m_modelBrowser;
	CMFCEditBrowseCtrl m_weaponBrowser;
	CMFCEditBrowseCtrl m_animationBrowser;
	afx_msg void OnChangeMfceditbrowseModel();
	afx_msg void OnEnChangeMfceditbrowseWeapon();
	afx_msg void OnEnChangeMfceditbrowseAni();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangedTreeWeapone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
