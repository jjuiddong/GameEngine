// TabPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "TabPanel.h"
#include "afxdialogex.h"
#include "HeightMapPanel.h"
#include "TerrainPanel.h"
#include "BrushPanel.h"
#include "ModelPanel.h"
#include "LightPanel.h"


CHeightMapPanel *g_heightMapPanel = NULL;
CTerrainPanel *g_terrainPanel = NULL;
CBrushPanel *g_brushPanel = NULL;
CModelPanel *g_modelPanel = NULL;
CLightPanel *g_lightPanel = NULL;


// CTabPanel 대화 상자입니다.
CTabPanel::CTabPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTabPanel::IDD, pParent)
{

}

CTabPanel::~CTabPanel()
{
	SAFE_DELETE(g_heightMapPanel);
	SAFE_DELETE(g_terrainPanel);
	SAFE_DELETE(g_brushPanel);
	SAFE_DELETE(g_modelPanel);
	SAFE_DELETE(g_lightPanel);
}

void CTabPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}


BEGIN_MESSAGE_MAP(CTabPanel, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTabPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTabPanel::OnBnClickedCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTabPanel::OnSelchangeTab1)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTabPanel 메시지 처리기입니다.


void CTabPanel::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}


void CTabPanel::OnBnClickedCancel()
{
	//CDialogEx::OnCancel();
}


BOOL CTabPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Tab.InsertItem(0,  L"Terrain");
	m_Tab.InsertItem(1, L"HeightMap");
	m_Tab.InsertItem(2,  L"Brush");
	m_Tab.InsertItem(3, L"Model");
	m_Tab.InsertItem(4,  L"Light");

	CRect cr;
	GetClientRect(cr);

	g_terrainPanel = new CTerrainPanel(this);
	g_terrainPanel->Create(CTerrainPanel::IDD, this);
	g_terrainPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_terrainPanel);

	g_heightMapPanel = new CHeightMapPanel(this);
	g_heightMapPanel->Create(CHeightMapPanel::IDD, this);
	g_heightMapPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_heightMapPanel);

	g_brushPanel = new CBrushPanel(this);
	g_brushPanel->Create(CBrushPanel::IDD, this);
	g_brushPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_brushPanel);

	g_modelPanel = new CModelPanel(this);
	g_modelPanel->Create(CModelPanel::IDD, this);
	g_modelPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_modelPanel);

	g_lightPanel = new CLightPanel(this);
	g_lightPanel->Create(CLightPanel::IDD, this);
	g_lightPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_lightPanel);


	BOOST_FOREACH (auto &panel, m_panels)
	{
		if (iObserver2 *observer = dynamic_cast<iObserver2*>(panel))
			cMapController::Get()->AddObserver(observer);
	}

	ShowPanel(0);

	return TRUE;
}


void CTabPanel::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowPanel(m_Tab.GetCurSel());
	*pResult = 0;
}


void CTabPanel::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_Tab.GetSafeHwnd())
	{
		m_Tab.MoveWindow(0, 0, cx, cy);

		BOOST_FOREACH (auto &panel, m_panels)
			panel->MoveWindow(CRect(0, 25, cx, cy));
	}
}


void CTabPanel::ShowPanel(int idx)
{
	BOOST_FOREACH (auto &panel, m_panels)
		panel->ShowWindow(SW_HIDE);

	if ((int)m_panels.size() > idx)
	{
		m_panels[ idx]->ShowWindow(SW_SHOW);	
		cMapController::Get()->ChangeEditMode((EDIT_MODE::TYPE)idx);
	}
}
