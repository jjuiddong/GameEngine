
// MapTool.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MapToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapToolApp
BEGIN_MESSAGE_MAP(CMapToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMapToolApp ����
CMapToolApp::CMapToolApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CMapToolApp theApp;

BOOL CMapToolApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));


	CMapToolDlg *dlg;
	dlg = new CMapToolDlg();
	m_pMainWnd = dlg;
	dlg->Create( CMapToolDlg::IDD, NULL);
	dlg->ShowWindow(SW_SHOW);
	dlg->MainLoop();


	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	SAFE_DELETE(dlg);
	//memmonitor::Cleanup();
	return FALSE;
}

