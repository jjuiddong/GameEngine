// MainPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "MainPanel.h"
#include "MainPanelTab.h"


// CMainPanel

CMainPanel::CMainPanel() :
	m_mainPanel(NULL)
{

}

CMainPanel::~CMainPanel()
{
	if (m_mainPanel)
	{
		m_mainPanel->DestroyWindow();
		delete m_mainPanel;
	}
}


BEGIN_MESSAGE_MAP(CMainPanel, CMiniFrameWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMainPanel 메시지 처리기입니다.


void CMainPanel::Init()
{
	CRect cr;
	GetClientRect(cr);

	m_mainPanel = new CMainPanelTab(this);
	m_mainPanel->Create( CMainPanelTab::IDD, this );
	m_mainPanel->MoveWindow(cr);
	m_mainPanel->ShowWindow(SW_SHOW);
}


void CMainPanel::OnSize(UINT nType, int cx, int cy)
{
	CMiniFrameWnd::OnSize(nType, cx, cy);
	if (m_mainPanel)
		m_mainPanel->MoveWindow(0, 0, cx, cy);
}
