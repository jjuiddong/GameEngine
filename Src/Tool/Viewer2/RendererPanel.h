#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CRendererPanel ��ȭ �����Դϴ�.

class CRendererPanel : public CDialogEx
								, public common::iObserver2
{
public:
	CRendererPanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRendererPanel();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RENDERER };

	virtual void Update(int type) override;


protected:
	void UpdateShaderFileList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CFileTreeCtrl m_shaderFiles;
	CString m_currentFileName;
	int m_RenderPass;
	afx_msg void OnEnChangeEditRenderPass();
	afx_msg void OnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonRefresh();
};
