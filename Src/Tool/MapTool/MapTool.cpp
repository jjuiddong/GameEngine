
// MapTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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


// CMapToolApp 생성
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
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));


	CMapToolDlg *dlg;
	dlg = new CMapToolDlg();
	m_pMainWnd = dlg;
	dlg->Create( CMapToolDlg::IDD, NULL);
	dlg->ShowWindow(SW_SHOW);
	dlg->MainLoop();


	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	SAFE_DELETE(dlg);
	//memmonitor::Cleanup();
	return FALSE;
}

