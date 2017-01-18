// LoadingDialog.cpp : ���� �����Դϴ�.
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


// CLoadingDialog �޽��� ó�����Դϴ�.


void CLoadingDialog::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CLoadingDialog::OnBnClickedCancel()
{

	CDialogEx::OnCancel();
}


CLoadingDialog *g_loadingDlg = NULL;


// �ε� ���̾�αװ� ȭ�鿡 �� ������ �޼��� ó���� �Ѵ�.
// �̷��� �ؾ� �ε� ���̾�αװ� �Ϻ��� ȭ�鿡 �� ��, �ε�ó���� �ϰ� �ȴ�.
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
		// �θ� �������� �߰��� ����ϰ� �Ѵ�.
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

