#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CRendererPanel 대화 상자입니다.

class CRendererPanel : public CDialogEx
								, public common::iObserver2
{
public:
	CRendererPanel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRendererPanel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RENDERER };

	virtual void Update(int type) override;


protected:
	void UpdateShaderFileList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
