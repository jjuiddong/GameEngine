
// Viewer2Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "Viewer2.h"
#include "Viewer2Dlg.h"
#include "ModelView.h"
#include "mmsystem.h"
#include "MainPanel.h"
#include "BoneDialog.h"
#include "AnimationController2.h"


#pragma comment( lib, "winmm.lib" )

CViewer2Dlg *g_viewerDlg;
CModelView *g_modelView;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CViewer2Dlg::CViewer2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CViewer2Dlg::IDD, pParent)
,	m_modelView(NULL)
,	m_aniController(NULL)
,	m_loop(true)
,	m_dxInit(false)
, m_WireFrame(FALSE)
, m_RenderBone(FALSE)
, m_RenderMesh(TRUE)
, m_RenderBoundingBox(FALSE)
, m_ShowSkyBox(FALSE)
,	m_dispMode(DISP_MODE::DISP_1024X768_LEFT)
{
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_viewerDlg = this;
}

CViewer2Dlg::~CViewer2Dlg()
{
	DestroyLoadingDialog();
	DestroyBoneDialog();
	m_aniController->DestroyWindow();
	delete m_aniController;
}

void CViewer2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_WIREFRAME, m_WireFrame);
	DDX_Check(pDX, IDC_CHECK_BONE, m_RenderBone);
	DDX_Check(pDX, IDC_CHECK_MESH, m_RenderMesh);
	DDX_Check(pDX, IDC_CHECK_BOUNDINGBOX, m_RenderBoundingBox);
	DDX_Check(pDX, IDC_CHECK_SKYBOX, m_ShowSkyBox);
	DDX_Control(pDX, IDC_COMBO_DISPLAY, m_dispCombo);
}

BEGIN_MESSAGE_MAP(CViewer2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CViewer2Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CViewer2Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_WIREFRAME, &CViewer2Dlg::OnBnClickedCheckWireframe)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_CHECK_BONE, &CViewer2Dlg::OnBnClickedCheckBone)
	ON_BN_CLICKED(IDC_CHECK_MESH, &CViewer2Dlg::OnBnClickedCheckMesh)
	ON_BN_CLICKED(IDC_CHECK_BOUNDINGBOX, &CViewer2Dlg::OnBnClickedCheckBoundingbox)
	ON_BN_CLICKED(IDC_CHECK_SKYBOX, &CViewer2Dlg::OnBnClickedCheckSkybox)
	ON_CBN_SELCHANGE(IDC_COMBO_DISPLAY, &CViewer2Dlg::OnCbnSelchangeComboDisplay)
END_MESSAGE_MAP()


// CViewer2Dlg �޽��� ó����

BOOL CViewer2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	//-----------------------------------------------------------------------------------------
	// Init
	DragAcceptFiles(TRUE);

	MoveWindow(CRect(0,0,REAL_WINDOW_WIDTH,REAL_WINDOW_HEIGHT));

	// Create Main Model View
	m_modelView = new CModelView();
	m_modelView->Create(NULL, _T("CView"), WS_CHILDWINDOW, 
		CRect(0,25, WINDOW_WIDTH, WINDOW_HEIGHT+25), this, 0);

	// Create Direct
	graphic::cRenderer::Get()->CreateDirectX(
		m_modelView->GetSafeHwnd(), WINDOW_WIDTH, WINDOW_HEIGHT);

	m_dxInit = true;
	m_modelView->Init();
	m_modelView->ShowWindow(SW_SHOW);
	g_modelView = m_modelView;


	// Create Main Panel
	{
		const int PANEL_WIDTH = 430;
		const int PANEL_HEIGHT = 800;

		m_mainPanel = new CMainPanel();
		const CString StrClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
			LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(COLOR_BTNFACE+1), 
			LoadIcon(NULL, IDI_APPLICATION) );

		m_mainPanel->CreateEx(0, StrClassName, L"Panel", 
			WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_THICKFRAME, CRect(0, 0, PANEL_WIDTH, PANEL_HEIGHT), this );

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


	// Create Animation Controller
	{
		m_aniController = new CAnimationController2(this);
		m_aniController->Create(CAnimationController2::IDD, this);
		m_aniController->MoveWindow(0, WINDOW_HEIGHT+20, WINDOW_WIDTH, 300);
		m_aniController->ShowWindow(SW_SHOW);
		cController::Get()->AddObserver(m_aniController);
	}

	// Loading Dialog �ʱ�ȭ.
	InitLoadingDialog(this);
	InitBoneDialog(this);


	// �ػ� �޺��ڽ� �ʱ�ȭ.
	m_dispCombo.InsertString(0, L"800 X 600 Right Align");
	m_dispCombo.InsertString(1, L"1024 X 768 Right Align");
	m_dispCombo.InsertString(2, L"800 X 600 Left Align");
	m_dispCombo.InsertString(3, L"1024 X 768 Left Align");
	switch (WINDOW_WIDTH)
	{
	case 800: m_dispCombo.SetCurSel(0); break;
	case 1024: m_dispCombo.SetCurSel(1); break;
	}


	cController::Get()->AddObserver(this);
	cController::Get()->SetViewerDlg(this);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CViewer2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CViewer2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CViewer2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CViewer2Dlg::OnBnClickedOk()
{
	m_loop = false;
	CDialogEx::OnOK();
}


void CViewer2Dlg::OnBnClickedCancel()
{
	m_loop = false;
	CDialogEx::OnCancel();
}


void CViewer2Dlg::MainLoop()
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

		if (m_dxInit)
		{
			if (m_modelView)
			{
				m_modelView->Update(t);
				m_modelView->Render();
			}

			if (m_aniController)
			{
				m_aniController->Update(t);
			}
		}

		Sleep(0);
	}
}


void CViewer2Dlg::OnDropFiles(HDROP hDropInfo)
{
	HDROP hdrop = hDropInfo;
	char filePath[ MAX_PATH];
	const UINT size = DragQueryFileA(hdrop, 0, filePath, MAX_PATH);
	if (size == 0) 
		return;// handle error...

	cController::Get()->LoadFile(filePath);
	
	CDialogEx::OnDropFiles(hDropInfo);
}


void CViewer2Dlg::OnBnClickedCheckWireframe()
{
	UpdateData();
	graphic::GetDevice()->SetRenderState(D3DRS_CULLMODE, !m_WireFrame? D3DCULL_CCW : D3DCULL_NONE);
	graphic::GetDevice()->SetRenderState(D3DRS_FILLMODE, !m_WireFrame? D3DFILL_SOLID : D3DFILL_WIREFRAME);
}


void CViewer2Dlg::OnBnClickedCheckBone()
{
	UpdateData();
	graphic::cCharacter *character = cController::Get()->GetCharacter();
	RET(!character);

	character->SetRenderBone(m_RenderBone? true : false);
}


void CViewer2Dlg::OnBnClickedCheckMesh()
{
	UpdateData();
	graphic::cCharacter *character = cController::Get()->GetCharacter();
	RET(!character);

	character->SetRenderMesh(m_RenderMesh? true : false);
}


void CViewer2Dlg::OnBnClickedCheckBoundingbox()
{
	UpdateData();
	graphic::cCharacter *character = cController::Get()->GetCharacter();
	RET(!character);

	if (graphic::MODEL_TYPE::RIGID == character->GetModelType())
		character->SetRenderBoundingBox(m_RenderBoundingBox? true : false);
	else
		character->SetRenderBoneBoundingBox(m_RenderBoundingBox? true : false);
}


void CViewer2Dlg::OnBnClickedCheckSkybox()
{
	UpdateData();
	if (m_modelView)
		m_modelView->ShowSkybox(m_ShowSkyBox? true : false);
}


// Observer Update
void CViewer2Dlg::Update(int type)
{
	int width, height;
	DISP_MODE::TYPE dispMode = (DISP_MODE::TYPE)m_dispCombo.GetCurSel();
	switch (dispMode)
	{
	case DISP_MODE::DISP_800X600_RIGHT: width = 800; height = 600; break; // 800X600 right align
	case DISP_MODE::DISP_1024X768_RIGHT: width = 1024; height = 768; break; // 1024X768 right align
	case DISP_MODE::DISP_800X600_LEFT: width = 800; height = 600; break; // 800X600 left align
	case DISP_MODE::DISP_1024X768_LEFT: width = 1024; height = 768; break; // 1024X768 left align
	default: return;
	}
	CRect mainR(0, 0,width+18, height+115);

	// ������Ʈ �� ���� �ִϸ��̼� ���¶�� AnimationController �� ��½�Ų��.
	if (graphic::cCharacter *character = cController::Get()->GetCharacter())
	{
		if (character->GetCurrentAnimation())
		{
			CRect wr;
			GetWindowRect(wr);
			MoveWindow(wr.left,wr.top, mainR.Width(), mainR.Height()+60);
		}
	}
}


void CViewer2Dlg::OnCbnSelchangeComboDisplay()
{
	int width, height;
	DISP_MODE::TYPE dispMode = (DISP_MODE::TYPE)m_dispCombo.GetCurSel();
	m_dispMode = dispMode;

	switch (dispMode)
	{
	case DISP_MODE::DISP_800X600_RIGHT: width = 800; height = 600; break; // 800X600 right align
	case DISP_MODE::DISP_1024X768_RIGHT: width = 1024; height = 768; break; // 1024X768 right align
	case DISP_MODE::DISP_800X600_LEFT: width = 800; height = 600; break; // 800X600 left align
	case DISP_MODE::DISP_1024X768_LEFT: width = 1024; height = 768; break; // 1024X768 left align
	default: return;
	}

	CRect curR;
	GetWindowRect(curR);

	CRect mainR(0, 0,width+18, height+115);
	CRect panelR;
	m_mainPanel->GetWindowRect(panelR);
	CRect animationCtrlR(0, height+20, width, height+300);

	const CRect edgeR( 
		max(0, min(curR.left, panelR.left)),
		max(0, min(curR.top, panelR.top)),
		max(curR.right, panelR.right),
		max(curR.bottom, panelR.bottom));

	switch (dispMode)
	{
	case DISP_MODE::DISP_800X600_RIGHT:
	case DISP_MODE::DISP_1024X768_RIGHT:
		mainR.OffsetRect(edgeR.left, edgeR.top);
		panelR = CRect(0, 0, panelR.Width(), panelR.Height());
		panelR.OffsetRect(mainR.left+mainR.Width(), mainR.top);
		break;

	case DISP_MODE::DISP_800X600_LEFT:
	case DISP_MODE::DISP_1024X768_LEFT:
		panelR = CRect(0, 0, panelR.Width(), panelR.Height());
		panelR.OffsetRect(edgeR.left, edgeR.top);
		mainR.OffsetRect(panelR.left+panelR.Width(), panelR.top);
		break;
	}

	MoveWindow(mainR);

	CRect modelViewR(0, 25, width, height+25);
	m_modelView->MoveWindow(modelViewR);

	// main panel
	// ���� ������ ���� ��ġ�ϰ� �Ѵ�.
	m_mainPanel->MoveWindow(panelR);

	// Animation Controller Positioning
	m_aniController->MoveWindow(animationCtrlR);

	// ���� �ִϸ��̼� ���¶�� AnimationController �� ��½�Ų��.
	if (graphic::cCharacter *character = cController::Get()->GetCharacter())
	{
		if (character->GetCurrentAnimation())
		{
			mainR.bottom += 60;
			MoveWindow(mainR);
		}
	}
}
