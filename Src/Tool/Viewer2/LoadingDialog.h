#pragma once

#include "resource.h"

class CLoadingDialog : public CDialogEx
{
public:
	CLoadingDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoadingDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOADING };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};


void InitLoadingDialog(CWnd *parent);
void ShowLoadingDialog();
void HideLoadingDialog();
void DestroyLoadingDialog();
