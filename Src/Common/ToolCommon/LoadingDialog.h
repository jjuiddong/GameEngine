#pragma once


class CLoadingDialog : public CWnd
{
public:
	CLoadingDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoadingDialog();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CWnd *m_pParentWnd;
	CStatic m_text;
	CFont m_font;
	afx_msg void OnClose();
};


void InitLoadingDialog(CWnd *parent);
void ShowLoadingDialog();
void HideLoadingDialog();
void DestroyLoadingDialog();
