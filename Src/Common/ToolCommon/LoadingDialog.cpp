// LoadingDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LoadingDialog.h"


CLoadingDialog::CLoadingDialog(CWnd* pParent /*=NULL*/)
:	m_pParentWnd(pParent)
{
}

CLoadingDialog::~CLoadingDialog()
{
}


BEGIN_MESSAGE_MAP(CLoadingDialog, CWnd)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


CLoadingDialog *g_loadingDlg = NULL;


void CLoadingDialog::OnClose()
{
	HideLoadingDialog();
}

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

	CString m_NameClass = AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(LTGRAY_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	const CRect rect(0, 0, 300, 40);
	if (g_loadingDlg->CreateEx(WS_EX_TOOLWINDOW, m_NameClass, L"LoadingDialog",
		WS_VISIBLE | WS_POPUP,
		CRect(0, 0, 300, 40),
		parent, 0 ))
	{
		g_loadingDlg->m_text.Create(L"Loading...", WS_VISIBLE | SS_CENTER,
			CRect(0, 00, 300, 80), g_loadingDlg);

		int fontHeight = 35;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = fontHeight;
		_tcscpy(lf.lfFaceName, _T( "Arial" ));
		g_loadingDlg->m_font.CreateFontIndirect(&lf);
		g_loadingDlg->m_text.SetFont(&g_loadingDlg->m_font);
	}
}


void ShowLoadingDialog()
{
	if (g_loadingDlg)
	{
		// �θ� �������� �߰��� ����ϰ� �Ѵ�.
		CRect pr, wr;
		g_loadingDlg->m_pParentWnd->GetWindowRect(pr);
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

