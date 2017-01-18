#pragma once

#include "resource.h"

class CLoadingDialog : public CDialogEx
{
public:
	CLoadingDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLoadingDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LOADING };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};


void InitLoadingDialog(CWnd *parent);
void ShowLoadingDialog();
void HideLoadingDialog();
void DestroyLoadingDialog();
