// TopPanel.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "TopPanel.h"
#include "TabPanel.h"


// CTopPanel
CTopPanel::CTopPanel()
	: m_tabPanel(NULL)
{

}

CTopPanel::~CTopPanel()
{
	SAFE_DELETE(m_tabPanel);
}


BEGIN_MESSAGE_MAP(CTopPanel, CMiniFrameWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTopPanel �޽��� ó�����Դϴ�.
void CTopPanel::Init()
{
	CRect cr;
	GetClientRect(cr);

	m_tabPanel = new CTabPanel(this);
	m_tabPanel->Create( CTabPanel::IDD, this );
	m_tabPanel->MoveWindow(cr);
	m_tabPanel->ShowWindow(SW_SHOW);
}


void CTopPanel::OnSize(UINT nType, int cx, int cy)
{
	CMiniFrameWnd::OnSize(nType, cx, cy);
	if (m_tabPanel)
		m_tabPanel->MoveWindow(0, 0, cx, cy);
}
