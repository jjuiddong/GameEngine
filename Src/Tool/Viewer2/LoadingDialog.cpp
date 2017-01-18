// LoadingDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "LoadingDialog.h"



CLoadingDialog::CLoadingDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoadingDialog::IDD, pParent)
{

}

CLoadingDialog::~CLoadingDialog()
{
}

void CLoadingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoadingDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoadingDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoadingDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLoadingDialog 메시지 처리기입니다.


void CLoadingDialog::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CLoadingDialog::OnBnClickedCancel()
{

	CDialogEx::OnCancel();
}


CLoadingDialog *g_loadingDlg = NULL;


// 로딩 다이얼로그가 화면에 뜰 때까지 메세지 처리를 한다.
// 이렇게 해야 로딩 다이얼로그가 완벽히 화면에 뜬 후, 로딩처리를 하게 된다.
void WaitForShowLoadingDialog()
{
	while (1)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0)) 
			{
				break;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			break;
		}
	}
}


void InitLoadingDialog(CWnd *parent)
{
		g_loadingDlg = new CLoadingDialog(parent);
		g_loadingDlg->Create(CLoadingDialog::IDD, parent);
}


void ShowLoadingDialog()
{
	if (g_loadingDlg)
	{
		// 부모 윈도우의 중간에 출력하게 한다.
		CRect pr, wr;
		g_loadingDlg->GetParent()->GetWindowRect(pr);
		g_loadingDlg->GetWindowRect(wr);

		const int x = pr.left + pr.Width()/2 - wr.Width()/2;
		const int y = pr.top + pr.Height()/2 - wr.Height()/2;
		g_loadingDlg->MoveWindow(x, y, wr.Width(), wr.Height());

		g_loadingDlg->ShowWindow(SW_SHOW);
		
		WaitForShowLoadingDialog();
	}
}


void HideLoadingDialog()
{
	if (g_loadingDlg)
	{
		g_loadingDlg->ShowWindow(SW_HIDE);
	}
}


void DestroyLoadingDialog()
{
	SAFE_DELETE(g_loadingDlg);
}

