// ArchebladePanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "ArchebladePanel.h"
#include "../../Common/Graphic/character/archebladecharacter.h"



// CArchebladePanel 대화 상자입니다.
CArchebladePanel::CArchebladePanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CArchebladePanel::IDD, pParent)
{

}

CArchebladePanel::~CArchebladePanel()
{
}

void CArchebladePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MODEL, m_modelTree);
	DDX_Control(pDX, IDC_TREE_WEAPONE, m_weaponTree);
	DDX_Control(pDX, IDC_TREE_ANIMATION, m_animationTree);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_MODEL, m_modelBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_WEAPON, m_weaponBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_ANI, m_animationBrowser);
}


BEGIN_MESSAGE_MAP(CArchebladePanel, CDialogEx)
	ON_BN_CLICKED(IDOK, &CArchebladePanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CArchebladePanel::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_MODEL, &CArchebladePanel::OnChangeMfceditbrowseModel)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_WEAPON, &CArchebladePanel::OnEnChangeMfceditbrowseWeapon)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_ANI, &CArchebladePanel::OnEnChangeMfceditbrowseAni)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CArchebladePanel::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MODEL, &CArchebladePanel::OnSelchangedTreeModel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ANIMATION, &CArchebladePanel::OnSelchangedTreeAnimation)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WEAPONE, &CArchebladePanel::OnSelchangedTreeWeapone)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CArchebladePanel 메시지 처리기입니다.


BOOL CArchebladePanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_modelBrowser.SetWindowText(L"./media/archeblade/");
	m_weaponBrowser.SetWindowText(L"./media/archeblade/");
	m_animationBrowser.SetWindowText(L"./media/archeblade/");

	UpdateModelTree();
	UpdateWeaponTree();
	UpdateAnimationTree();

	return TRUE;
}


void CArchebladePanel::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}


void CArchebladePanel::OnBnClickedCancel()
{
	//CDialogEx::OnCancel();
}


void CArchebladePanel::Update(int type)
{

}


void CArchebladePanel::UpdateModelTree()
{
	CString fileName;
	m_modelBrowser.GetWindowText(fileName);
	string directory = wstr2str((wstring)fileName);

	// 파일 찾기.
	list<string> extList;
	extList.push_back("dat");
	m_modelTree.Update( directory, extList );
	m_modelTree.ExpandAll();
}


void CArchebladePanel::UpdateWeaponTree()
{
	CString fileName;
	m_weaponBrowser.GetWindowText(fileName);
	string directory = wstr2str((wstring)fileName);

	// 파일 찾기.
	list<string> extList;
	extList.push_back("dat");
	m_weaponTree.Update( directory, extList );
	m_weaponTree.ExpandAll();
}


void CArchebladePanel::UpdateAnimationTree()
{
	CString fileName;
	m_animationBrowser.GetWindowText(fileName);
	string directory = wstr2str((wstring)fileName);

	// 파일 찾기.
	list<string> extList;
	extList.push_back("ani");
	m_animationTree.Update( directory, extList );
	m_animationTree.ExpandAll();
}


void CArchebladePanel::OnChangeMfceditbrowseModel()
{
	UpdateModelTree();
}


void CArchebladePanel::OnEnChangeMfceditbrowseWeapon()
{
	UpdateWeaponTree();
}


void CArchebladePanel::OnEnChangeMfceditbrowseAni()
{
	UpdateAnimationTree();
}


void CArchebladePanel::OnBnClickedButtonRefresh()
{
	UpdateModelTree();
	UpdateWeaponTree();
	UpdateAnimationTree();
}


// 모델 선택
void CArchebladePanel::OnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_modelTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;
	
	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}


// 무기 선택
void CArchebladePanel::OnSelchangedTreeWeapone(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if (graphic::cArchebladeCharacter *character = 
		dynamic_cast<graphic::cArchebladeCharacter*>(cController::Get()->GetCharacter()))
	{
		const string fileName = m_weaponTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
		if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
			return;

		character->LoadWeapon(fileName);
	}

}


// 애니메이션 선택
void CArchebladePanel::OnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_animationTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}


void CArchebladePanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(*this, m_modelTree, cx, cy);
	MoveChildCtrlWindow(*this, m_weaponTree, cx, cy);
	MoveChildCtrlWindow(*this, m_animationTree, cx, cy);
	MoveChildCtrlWindow(*this, m_modelBrowser, cx-100, cy);
	MoveChildCtrlWindow(*this, m_weaponBrowser, cx-100, cy);
	MoveChildCtrlWindow(*this, m_animationBrowser, cx-110, cy);
}
