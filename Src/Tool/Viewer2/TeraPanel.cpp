// TeraPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "TeraPanel.h"
#include "../../Common/Graphic/character/teracharacter.h"


// CTeraPanel 대화 상자입니다.
CTeraPanel::CTeraPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTeraPanel::IDD, pParent)
{

}

CTeraPanel::~CTeraPanel()
{
}

void CTeraPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_BODY, m_bodyBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_HAND, m_handBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_LEG, m_legBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_FACE, m_faceBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_HAIR, m_hairBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_TAIL, m_tailBrowser);
	DDX_Control(pDX, IDC_TREE_FILE, m_fileTree);
	DDX_Control(pDX, IDC_TREE_ANIMATION, m_animationTree);
}


BEGIN_MESSAGE_MAP(CTeraPanel, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTeraPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTeraPanel::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIO_BODY, &CTeraPanel::OnBnClickedRadioBody)
	ON_BN_CLICKED(IDC_RADIO_HAND, &CTeraPanel::OnBnClickedRadioHand)
	ON_BN_CLICKED(IDC_RADIO_LEG, &CTeraPanel::OnBnClickedRadioLeg)
	ON_BN_CLICKED(IDC_RADIO_FACE, &CTeraPanel::OnBnClickedRadioFace)
	ON_BN_CLICKED(IDC_RADIO_HAIR, &CTeraPanel::OnBnClickedRadioHair)
	ON_BN_CLICKED(IDC_RADIO_TAIL, &CTeraPanel::OnBnClickedRadioTail)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FILE, &CTeraPanel::OnSelchangedTreeFile)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ANIMATION, &CTeraPanel::OnSelchangedTreeAnimation)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CTeraPanel::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// CTeraPanel 메시지 처리기입니다.


void CTeraPanel::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}


void CTeraPanel::OnBnClickedCancel()
{
	//CDialogEx::OnCancel();
}


BOOL CTeraPanel::OnInitDialog()
{
	__super::OnInitDialog();

	UpdateModelTree();
	UpdateAnimationTree();

	m_selectModelType = 0;
	((CButton*)GetDlgItem(IDC_RADIO_BODY))->SetCheck(1);
	
	return TRUE;
}


void CTeraPanel::Update(int type)
{
	
}


void CTeraPanel::UpdateModelTree()
{
	list<string> extList;
	extList.push_back( "dat" );
	m_fileTree.Update( "./media/tera/", extList );
	m_fileTree.ExpandAll();
}


void CTeraPanel::UpdateAnimationTree()
{
	list<string> extList;
	extList.push_back( "ani" );
	m_animationTree.Update( "./media/tera/", extList );
	m_animationTree.ExpandAll();
}


void CTeraPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(*this, m_bodyBrowser, cx-50, cy);
	MoveChildCtrlWindow(*this, m_handBrowser, cx-50, cy);
	MoveChildCtrlWindow(*this, m_legBrowser, cx-50, cy);
	MoveChildCtrlWindow(*this, m_faceBrowser, cx-50, cy);
	MoveChildCtrlWindow(*this, m_hairBrowser, cx-50, cy);
	MoveChildCtrlWindow(*this, m_tailBrowser, cx-50, cy);

	MoveChildCtrlWindow(*this, m_fileTree, cx, cy);
}


void CTeraPanel::OnBnClickedRadioBody()
{
	m_selectModelType = 0;
}


void CTeraPanel::OnBnClickedRadioHand()
{
	m_selectModelType = 1;
}


void CTeraPanel::OnBnClickedRadioLeg()
{
	m_selectModelType = 2;
}


void CTeraPanel::OnBnClickedRadioFace()
{
	m_selectModelType = 3;
}


void CTeraPanel::OnBnClickedRadioHair()
{
	m_selectModelType = 4;
}


void CTeraPanel::OnBnClickedRadioTail()
{
	m_selectModelType = 5;
}


void CTeraPanel::OnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_fileTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	ShowLoadingDialog();

	if (graphic::cTeraCharacter *character = dynamic_cast<graphic::cTeraCharacter*>(
		cController::Get()->GetCharacter()))
	{
		switch (m_selectModelType)
		{
		case 0: character->SetBodyModel(fileName); break;
		case 1: character->SetHandModel(fileName); break;
		case 2: character->SetLegModel(fileName); break;
		case 3: character->SetFaceModel(fileName); break;
		case 4: character->SetHairModel(fileName); break;
		case 5: character->SetTailModel(fileName); break;
		}
	}

	switch (m_selectModelType)
	{
	case 0: m_bodyBrowser.SetWindowText( str2wstr(fileName).c_str() ); break;
	case 1: m_handBrowser.SetWindowText( str2wstr(fileName).c_str() ); break;
	case 2: m_legBrowser.SetWindowText( str2wstr(fileName).c_str() ); break;
	case 3: m_faceBrowser.SetWindowText( str2wstr(fileName).c_str() ); break;
	case 4: m_hairBrowser.SetWindowText( str2wstr(fileName).c_str() ); break;
	case 5: m_tailBrowser.SetWindowText( str2wstr(fileName).c_str() ); break;
	}

	cController::Get()->SendUpdate();

	HideLoadingDialog();
}


void CTeraPanel::OnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_animationTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}


void CTeraPanel::OnBnClickedButtonRefresh()
{
	UpdateModelTree();
	UpdateAnimationTree();
}
