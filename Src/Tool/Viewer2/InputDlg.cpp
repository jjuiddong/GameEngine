// InputDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "InputDlg.h"


// CInputDlg 대화 상자입니다.

CInputDlg::CInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputDlg::IDD, pParent)
	, m_label(_T(""))
	, m_value(_T(""))
{

}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_label);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_value);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialogEx)
END_MESSAGE_MAP()


// CInputDlg 메시지 처리기입니다.
