// FilePanel.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "FilePanel.h"


// CFilePanel ��ȭ �����Դϴ�.
CFilePanel::CFilePanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CFilePanel::IDD, pParent)
	, m_textModelFile(_T(""))
	, m_textAnimationFiles(_T(""))
{

}

CFilePanel::~CFilePanel()
{
}

void CFilePanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MODEL_FILES, m_textModelFile);
	DDX_Text(pDX, IDC_STATIC_ANIMATION_FILES, m_textAnimationFiles);
	DDX_Control(pDX, IDC_TREE_MODEL, m_modelTree);
	DDX_Control(pDX, IDC_TREE_ANIMATION, m_animationTree);
}


BEGIN_MESSAGE_MAP(CFilePanel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CFilePanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFilePanel::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CFilePanel::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MODEL, &CFilePanel::OnSelchangedTreeModel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ANIMATION, &CFilePanel::OnSelchangedTreeAnimation)
END_MESSAGE_MAP()


BOOL CFilePanel::OnInitDialog()
{
	__super::OnInitDialog();

	// ./media  �������� *.dat, *.ani ������ ã�Ƽ� �������Ѵ�.
	UpdateModelFiles();
	UpdateAnimationFiles();

	return TRUE; 
}


void CFilePanel::Update(int type)
{
	// ���� �ϴ� �� ����.
}


// CFilePanel �޽��� ó�����Դϴ�.
void CFilePanel::OnBnClickedOk()
{
}

void CFilePanel::OnBnClickedCancel()
{
}


// ./media  �������� *.dat ������ ã�Ƽ� �������Ѵ�.
void CFilePanel::UpdateModelFiles()
{
	// ���� ã��.
	list<string> extList;
	extList.push_back("dat");
	m_modelTree.Update( "./media/", extList );

	m_textModelFile.Format( L"Model Files : %d", m_modelTree.GetFileCount());
	UpdateData(FALSE);
}


// ./media  �������� *.ani ������ ã�Ƽ� �������Ѵ�.
void CFilePanel::UpdateAnimationFiles()
{
	// ���� ã��.
	list<string> extList;
	extList.push_back("ani");
	m_animationTree.Update( "./media/", extList);

	m_textAnimationFiles.Format( L"Animation Files : %d", m_animationTree.GetFileCount());
	UpdateData(FALSE);
}



void CFilePanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(m_modelTree, cx, cy);
	MoveChildCtrlWindow(m_animationTree, cx, cy);	
}


void CFilePanel::OnBnClickedButtonRefresh()
{
	UpdateModelFiles();
	UpdateAnimationFiles();
	graphic::cResourceManager::Get()->ReloadFile();
}


void CFilePanel::OnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_modelTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}


void CFilePanel::OnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_modelTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}
