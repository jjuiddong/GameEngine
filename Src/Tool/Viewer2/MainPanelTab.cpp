// MainPanelTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "MainPanelTab.h"
#include "ModelPanel.h"
#include "AnimationPanel.h"
#include "FilePanel.h"
#include "ArchebladePanel.h"
#include "TeraPanel.h"
#include "LightPanel.h"
#include "RendererPanel.h"
#include "Sc2Panel.h"


CModelPanel *g_modelPanel;
CAnimationPanel *g_aniPanel;
CFilePanel *g_filePanel;
CArchebladePanel *g_archePanel;
CTeraPanel *g_teraPanel;
CLightPanel *g_lightPanel;
CRendererPanel *g_rendererPanel;
CSc2Panel *g_sc2Panel;

// CMainPanelTab 대화 상자입니다.

CMainPanelTab::CMainPanelTab(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainPanelTab::IDD, pParent)
{

}

CMainPanelTab::~CMainPanelTab()
{
	BOOST_FOREACH (auto &panel, m_panels)
	{
		SAFE_DELETE(panel);
	}
	m_panels.clear();
}

void CMainPanelTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}


BEGIN_MESSAGE_MAP(CMainPanelTab, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMainPanelTab::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMainPanelTab::OnBnClickedCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMainPanelTab::OnSelchangeTab)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMainPanelTab 메시지 처리기입니다.


void CMainPanelTab::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}


void CMainPanelTab::OnBnClickedCancel()
{
	//CDialogEx::OnCancel();
}


BOOL CMainPanelTab::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Tab.InsertItem(0, L"Model File");
	m_Tab.InsertItem(1, L"Model");
	m_Tab.InsertItem(2, L"Animation");
	m_Tab.InsertItem(3, L"Renderer");
	m_Tab.InsertItem(4, L"Light");
	m_Tab.InsertItem(5, L"ArcheBlade");
	m_Tab.InsertItem(6, L"Tera");
	m_Tab.InsertItem(7, L"Sc2");

	CRect cr;
	GetClientRect(cr);

	g_filePanel = new CFilePanel(this);
	g_filePanel->Create(CFilePanel::IDD, this);
	g_filePanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_filePanel);

	g_modelPanel = new CModelPanel(this);
	g_modelPanel->Create(CModelPanel::IDD, this);
	g_modelPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_modelPanel);

	g_aniPanel = new CAnimationPanel(this);
	g_aniPanel->Create(CAnimationPanel::IDD, this);
	g_aniPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_aniPanel);

	g_rendererPanel = new CRendererPanel(this);
	g_rendererPanel->Create(CRendererPanel::IDD, this);
	g_rendererPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_rendererPanel);

	g_lightPanel = new CLightPanel(this);
	g_lightPanel->Create(CLightPanel::IDD, this);
	g_lightPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_lightPanel);

	g_archePanel = new CArchebladePanel(this);
	g_archePanel->Create(CArchebladePanel::IDD, this);
	g_archePanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_archePanel);

	g_teraPanel = new CTeraPanel(this);
	g_teraPanel->Create(CTeraPanel::IDD, this);
	g_teraPanel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_teraPanel);

	g_sc2Panel = new CSc2Panel(this);
	g_sc2Panel->Create(CSc2Panel::IDD, this);
	g_sc2Panel->MoveWindow(CRect(0, 25, cr.Width(), cr.Height()));
	m_panels.push_back(g_sc2Panel);


	BOOST_FOREACH (auto &panel, m_panels)
	{
		if (iObserver2 *observer = dynamic_cast<iObserver2*>(panel))
			cController::Get()->AddObserver(observer);
	}

	ShowPanel(0);

	return TRUE;
}


void CMainPanelTab::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowPanel(m_Tab.GetCurSel());
	cController::Get()->ChangePanel(m_Tab.GetCurSel());
	*pResult = 0;
}


void CMainPanelTab::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_Tab.GetSafeHwnd())
	{
		m_Tab.MoveWindow(0, 0, cx, cy);

		BOOST_FOREACH (auto &panel, m_panels)
			panel->MoveWindow(CRect(0, 25, cx, cy));
	}
}


void CMainPanelTab::ShowPanel(int idx)
{
	BOOST_FOREACH (auto &panel, m_panels)
		panel->ShowWindow(SW_HIDE);

	if ((int)m_panels.size() > idx)
		m_panels[ idx]->ShowWindow(SW_SHOW);
}
