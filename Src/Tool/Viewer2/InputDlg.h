#pragma once


// CInputDlg 대화 상자입니다.

class CInputDlg : public CDialogEx
{
public:
	CInputDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInputDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_label;
	CString m_value;
};
