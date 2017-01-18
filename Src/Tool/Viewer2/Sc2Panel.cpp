// Sc2Panel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "Sc2Panel.h"
#include "afxdialogex.h"


// CSc2Panel 대화 상자입니다.
CSc2Panel::CSc2Panel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CSc2Panel::IDD, pParent)
{
}

CSc2Panel::~CSc2Panel()
{
}

void CSc2Panel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SC2_FILES, m_modelFileTree);
	DDX_Control(pDX, IDC_TREE_SC2ANI_FILES, m_aniFileTree);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_MODEL, m_modelBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_ANI, m_animationBrowser);
}


BEGIN_MESSAGE_MAP(CSc2Panel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CSc2Panel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSc2Panel::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SC2_FILES, &CSc2Panel::OnTvnSelchangedTreeSc2Files)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SC2ANI_FILES, &CSc2Panel::OnTvnSelchangedTreeSc2aniFiles)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSc2Panel 메시지 처리기입니다.


void CSc2Panel::OnBnClickedOk()
{
	//CPanelBase::OnOK();
}


void CSc2Panel::OnBnClickedCancel()
{
	//CPanelBase::OnCancel();
}


BOOL CSc2Panel::OnInitDialog()
{
	__super::OnInitDialog();

	m_modelBrowser.SetWindowText(L"./media/sc2/");
	m_animationBrowser.SetWindowText(L"./media/sc2/");

	UpdateModelTree();
	UpdateAnimationTree();

	return TRUE;
}


void CSc2Panel::UpdateModelTree()
{
	CString fileName;
	m_modelBrowser.GetWindowText(fileName);
	string directory = wstr2str((wstring)fileName);

	// 파일 찾기.
	list<string> extList;
	extList.push_back("dat");
	m_modelFileTree.Update( directory, extList );
	m_modelFileTree.ExpandAll();
}


void CSc2Panel::UpdateAnimationTree(const string &modelFileName)
{
	CString fileName;
	m_animationBrowser.GetWindowText(fileName);
	string directory = wstr2str((wstring)fileName);

	// 파일 찾기.
	list<string> extList;
	extList.push_back("ani");
	m_aniFileTree.Update( directory, extList, common::GetFileNameExceptExt(modelFileName));
	m_aniFileTree.ExpandAll();
}


// 업데이트.
void CSc2Panel::Update(int type)
{
	switch (type)
	{
	case NOTIFY_MSG::UPDATE_MODEL:
		{
		}
		break;
	}
}


void CSc2Panel::OnTvnSelchangedTreeSc2Files(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_modelFileTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);

	UpdateAnimationTree(fileName);
}


void CSc2Panel::OnTvnSelchangedTreeSc2aniFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_aniFileTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	ShowLoadingDialog();
	cController::Get()->LoadFile(fileName);
}


void CSc2Panel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(m_modelFileTree, cx, cy);
	MoveChildCtrlWindow(m_aniFileTree, cx, cy);
	MoveChildCtrlWindow(m_modelBrowser, cx-100, cy);
	MoveChildCtrlWindow(m_animationBrowser, cx-110, cy);
}
