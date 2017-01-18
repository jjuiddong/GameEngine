
// Viewer2.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "Viewer2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewer2App

BEGIN_MESSAGE_MAP(CViewer2App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CViewer2App 생성

CViewer2App::CViewer2App()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

// 유일한 CViewer2App 개체입니다.
CViewer2App theApp;


// CViewer2App 초기화
BOOL CViewer2App::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();
	CShellManager *pShellManager = new CShellManager;
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	CViewer2Dlg *pdlg = new CViewer2Dlg();
	pdlg->Create(CViewer2Dlg::IDD);
	m_pMainWnd = pdlg;
	pdlg->ShowWindow(SW_SHOW);

	pdlg->MainLoop();

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	pdlg->DestroyWindow();
	delete pdlg;
	cController::Release();
	graphic::ReleaseRenderer();
	//memmonitor::Cleanup();
	return FALSE;
}

