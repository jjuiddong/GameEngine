// RendererPanel.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "RendererPanel.h"
#include "afxdialogex.h"


using namespace graphic;


// CRendererPanel ��ȭ �����Դϴ�.
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


// CRendererPanel �޽��� ó�����Դϴ�.


void CRendererPanel::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnCancel();
}


void CRendererPanel::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
}


BOOL CRendererPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateShaderFileList();
	return TRUE;
}


// ���̴� ���� ����Ʈ�� ������Ʈ �Ѵ�.
void CRendererPanel::UpdateShaderFileList()
{
	// ���� ã��.
	list<string> extList;
	extList.push_back("fx");
	m_shaderFiles.Update( "./media/shader/", extList);
	m_shaderFiles.ExpandAll();
}


// ������Ʈ.
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
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == "."))
		return;

	cController::Get()->GetCharacter()->SetShader( 
		cResourceManager::Get()->LoadShader(fileName) );

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
