
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

graphic::cRenderer *g_renderer = NULL;


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


// CMapToolDlg 皋矫瘤 贸府扁
BOOL CMapToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL); 

	graphic::cResourceManager::Get()->SetMediaDirectory( "../media/");

	MoveWindow(CRect(0,0, REAL_WINDOW_WIDTH, REAL_WINDOW_HEIGHT));

	// Create Main Model View
	m_mapView = new CMapView();
	m_mapView->Create(NULL, _T("CView"), WS_CHILDWINDOW, 
		CRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT), this, 0);
		//CRect(0,25, VIEW_WIDTH, VIEW_HEIGHT+25), this, 0);

	m_mapView->Init();
	m_mapView->ShowWindow(SW_SHOW);

	// TopPanel 积己.
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

	return TRUE;
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


void CMapToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


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

