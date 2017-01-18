#pragma once
#include "afxcmn.h"
#include "PanelBase.h"


// CAnimationPanel ��ȭ �����Դϴ�.

class CAnimationPanel : public CPanelBase
									   , public common::iObserver2
{
public:
	CAnimationPanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAnimationPanel();
	enum { IDD = IDD_DIALOG_ANIMATION };

	virtual void Update(int type) override;


protected:
	void UpdateAnimationInfo();
	void UpdateAnimationTree(bool showDetails=false);

	void InsertAnimationInfo(HTREEITEM hItem, const graphic::sRawAni &ani);
	void InsertAnimationDetailInfoBone(HTREEITEM hItem, const graphic::sRawBone &bone);
	void InsertAnimationDetailInfoAni(HTREEITEM hItem, const graphic::sRawAni &ani);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_FilePath;
	CString m_FileName;
	int m_StartFrame;
	int m_EndFrame;
	CTreeCtrl m_AniTree;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnPanelDetailanimationinfo();
};
