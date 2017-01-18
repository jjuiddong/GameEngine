
// MapToolDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MapTool.h"
#include "MapToolDlg.h"
#include "afxdialogex.h"
#include <MMSystem.h>
#include "MapView.h"
#include "TopPanel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace graphic;


CMapToolDlg::CMapToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMapToolDlg::IDD, pParent)
,	m_loop(true)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMapToolDlg::~CMapToolDlg()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	graphic::ReleaseRenderer();
	cMapController::Release();
	DestroyLoadingDialog();
}

void CMapToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMapToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMapToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMapToolDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMapToolDlg 메시지 처리기

BOOL CMapToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL); 

	graphic::cResourceManager::Get()->SetMediaDirectory( "./media/");

	MoveWindow(CRect(0,0,REAL_WINDOW_WIDTH,REAL_WINDOW_HEIGHT));

	// Create Main Model View
	m_mapView = new CMapView();
	m_mapView->Create(NULL, _T("CView"), WS_CHILDWINDOW, 
		CRect(0,25, VIEW_WIDTH, VIEW_HEIGHT+25), this, 0);

	// Create Direct
	graphic::cRenderer::Get()->CreateDirectX(
		m_mapView->GetSafeHwnd(), VIEW_WIDTH, VIEW_HEIGHT);

	m_mapView->Init();
	m_mapView->ShowWindow(SW_SHOW);


	// TopPanel 생성.
	{
		const int PANEL_WIDTH = 400;
		const int PANEL_HEIGHT = 800;

		m_mainPanel = new CTopPanel();
		const CString StrClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
			LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(COLOR_BTNFACE+1), 
			LoadIcon(NULL, IDI_APPLICATION) );

		m_mainPanel->CreateEx(0, StrClassName, L"Panel", 
			WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_THICKFRAME, 
			CRect(0, 0, PANEL_WIDTH, PANEL_HEIGHT), this );

		m_mainPanel->Init();

		// Main Panel Positioning
		{
			CRect panelR;
			m_mainPanel->GetWindowRect(panelR);

			const int screenCX = GetSystemMetrics(SM_CXSCREEN);
			const int screenCY = GetSystemMetrics(SM_CYSCREEN);
			int x = screenCX/2 - REAL_WINDOW_WIDTH/2 + REAL_WINDOW_WIDTH - panelR.Width()/2;
			const int y = screenCY/2 - REAL_WINDOW_HEIGHT/2;

			if ((x+panelR.Width()) > screenCX)
				x = screenCX - panelR.Width();

			m_mainPanel->MoveWindow(x, y, panelR.Width(), panelR.Height());

			// Main Dialog RePositioning
			int px = screenCX/2 - REAL_WINDOW_WIDTH/2 - panelR.Width()/2;			
			px = max(0, px);
			MoveWindow(px, y, REAL_WINDOW_WIDTH,REAL_WINDOW_HEIGHT);
		}

		m_mainPanel->ShowWindow(SW_SHOW);
	}


	InitLoadingDialog(this);
	HideLoadingDialog();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CMapToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMapToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMapToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMapToolDlg::OnBnClickedOk()
{
	m_loop = false;
	CDialogEx::OnOK();
}


void CMapToolDlg::OnBnClickedCancel()
{
	m_loop = false;
	CDialogEx::OnCancel();
}


void CMapToolDlg::MainLoop()
{
	while (m_loop)
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

		const int curT = timeGetTime();
		static int oldT = curT;
		const int elapseT = curT - oldT;
		const float t = elapseT * 0.001f;
		oldT = curT;

		if (m_mapView)
		{
			m_mapView->Update(t);
			m_mapView->Render();
		}

		Sleep(0);
	}

}

