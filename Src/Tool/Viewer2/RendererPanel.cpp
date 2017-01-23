// RendererPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "RendererPanel.h"
#include "afxdialogex.h"


using namespace graphic;


// CRendererPanel 대화 상자입니다.
CRendererPanel::CRendererPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRendererPanel::IDD, pParent)
,	m_currentFileName(_T(""))
, m_RenderPass(0)
{
}

CRendererPanel::~CRendererPanel()
{
}

void CRendererPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SHADER_FILES, m_shaderFiles);
	DDX_Text(pDX, IDC_STATIC_SHADER_FILENAME, m_currentFileName);
	DDX_Text(pDX, IDC_EDIT_RENDER_PASS, m_RenderPass);
}


BEGIN_MESSAGE_MAP(CRendererPanel, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CRendererPanel::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CRendererPanel::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_RENDER_PASS, &CRendererPanel::OnEnChangeEditRenderPass)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SHADER_FILES, &CRendererPanel::OnSelchangedTreeModel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CRendererPanel::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// CRendererPanel 메시지 처리기입니다.


void CRendererPanel::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
}


void CRendererPanel::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


BOOL CRendererPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateShaderFileList();
	return TRUE;
}


// 셰이더 파일 리스트를 업데이트 한다.
void CRendererPanel::UpdateShaderFileList()
{
	// 파일 찾기.
	list<string> extList;
	extList.push_back("fx");
	m_shaderFiles.Update( "../media/shader/", extList);
	m_shaderFiles.ExpandAll();
}


// 업데이트.
void CRendererPanel::Update(int type)
{
	switch (type)
	{
	case NOTIFY_MSG::UPDATE_MODEL:
		{
			if (cShader *shader = cController::Get()->GetCharacter()->GetShader())
			{
				m_currentFileName = str2wstr(shader->GetFileName()).c_str();
				UpdateData(FALSE);
			}
		}
		break;
	}
}


void CRendererPanel::OnEnChangeEditRenderPass()
{

}


void CRendererPanel::OnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_shaderFiles.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == ".") || (fileName == "..") || (fileName == ".."))
		return;

	cController::Get()->GetCharacter()->SetShader( 
		cResourceManager::Get()->LoadShader(*g_renderer, fileName) );

	cController::Get()->SendUpdate(NOTIFY_MSG::UPDATE_SHADER);

	m_currentFileName = str2wstr(fileName).c_str();
	UpdateData(FALSE);
}


void CRendererPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(*this, m_shaderFiles, cx, cy);	
}


void CRendererPanel::OnBnClickedButtonRefresh()
{
	UpdateShaderFileList();
}
