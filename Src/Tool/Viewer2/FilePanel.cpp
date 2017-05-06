// FilePanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "FilePanel.h"


// CFilePanel 대화 상자입니다.
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

	// ./media  폴더에서 *.dat, *.ani 파일을 찾아서 리스팅한다.
	UpdateModelFiles();
	UpdateAnimationFiles();

	return TRUE; 
}


void CFilePanel::Update(int type)
{
	// 아직 하는 일 없음.
}


// CFilePanel 메시지 처리기입니다.
void CFilePanel::OnBnClickedOk()
{
}

void CFilePanel::OnBnClickedCancel()
{
}


// ./media  폴더에서 *.dat 파일을 찾아서 리스팅한다.
void CFilePanel::UpdateModelFiles()
{
	// 파일 찾기.
	list<string> extList;
	extList.push_back("dat");
	m_modelTree.Update( "../media/", extList );

	m_textModelFile.Format( L"Model Files : %d", m_modelTree.GetFileCount());
	UpdateData(FALSE);
}


// ./media  폴더에서 *.ani 파일을 찾아서 리스팅한다.
void CFilePanel::UpdateAnimationFiles()
{
	// 파일 찾기.
	list<string> extList;
	extList.push_back("ani");
	m_animationTree.Update( "../media/", extList);

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
}


void CFilePanel::OnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_modelTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == ".") || (fileName == ".."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}


void CFilePanel::OnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_modelTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == ".") || (fileName == ".."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}
