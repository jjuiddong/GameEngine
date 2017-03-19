// HeightMapPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "HeightMapPanel.h"
#include "afxdialogex.h"


// CHeightMapPanel 대화 상자입니다.
CHeightMapPanel::CHeightMapPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CHeightMapPanel::IDD, pParent)
,	m_heightMap(NULL)
,	m_texture(NULL)
, m_heightFactor(0)
, m_uvFactor(0)
{

}

CHeightMapPanel::~CHeightMapPanel()
{
}

void CHeightMapPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_HEIGHT_FACTOR, m_heightSlider);
	DDX_Text(pDX, IDC_EDIT_HEIGHT_FACTOR, m_heightFactor);
	DDX_Control(pDX, IDC_SLIDER_UV_FACTOR, m_uvSlider);
	DDX_Text(pDX, IDC_EDIT_UV_FACTOR, m_uvFactor);
	DDX_Control(pDX, IDC_TREE_HEIGHTMAP, m_heightmapTree);
	DDX_Control(pDX, IDC_TREE_TEXTURE, m_textureTree);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_HEIGHTMAP, m_heightmapBrowser);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_TEXTURE, m_textureBrowser);
}


BEGIN_MESSAGE_MAP(CHeightMapPanel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CHeightMapPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHeightMapPanel::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HEIGHT_FACTOR, &CHeightMapPanel::OnNMCustomdrawSliderHeightFactor)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT_FACTOR, &CHeightMapPanel::OnEnChangeEditHeightFactor)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_UV_FACTOR, &CHeightMapPanel::OnNMCustomdrawSliderUvFactor)
	ON_EN_CHANGE(IDC_EDIT_UV_FACTOR, &CHeightMapPanel::OnEnChangeEditUvFactor)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CHeightMapPanel::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_HEIGHTMAP, &CHeightMapPanel::OnTvnSelchangedTreeHeightmap)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TEXTURE, &CHeightMapPanel::OnTvnSelchangedTreeTexture)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_HEIGHTMAP, &CHeightMapPanel::OnEnChangeMfceditbrowseHeightmap)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_TEXTURE, &CHeightMapPanel::OnEnChangeMfceditbrowseTexture)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CHeightMapPanel 메시지 처리기입니다.
BOOL CHeightMapPanel::OnInitDialog()
{
	CPanelBase::OnInitDialog();

	m_heightSlider.SetRange(0, 1000);
	m_uvSlider.SetRange(0, 100000);

	m_heightmapBrowser.EnableFileBrowseButton(_T("HeightMap"), 
		_T("Image files|*.jpg;*.png;*.bmp|All files|*.*||"));

	m_textureBrowser.EnableFileBrowseButton(_T("Texture"), 
		_T("Image files|*.jpg;*.png;*.bmp|All files|*.*||"));

	UpdateHeightMapList();
	UpdateTextureList();
	UpdateTerrainInfo();

	return TRUE;
}


void CHeightMapPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Graphics *graph = Graphics::FromHDC(dc.GetSafeHdc());

	if (m_heightMap)
	{
		// 텍스쳐 출력.
		if (CWnd *pictureCtlr = GetDlgItem(IDC_STATIC_TEXTURE))
		{
			CRect cr;
			pictureCtlr->GetWindowRect(cr);
			ScreenToClient(cr);

			const Rect dest(cr.left, cr.top, cr.Width(), cr.Height());
			graph->DrawImage(m_heightMap, dest );
		}
	}

	if (m_texture)
	{
		// 텍스쳐 출력.
		if (CWnd *pictureCtlr = GetDlgItem(IDC_STATIC_TEXTURE2))
		{
			CRect cr;
			pictureCtlr->GetWindowRect(cr);
			ScreenToClient(cr);

			const Rect dest(cr.left, cr.top, cr.Width(), cr.Height());
			graph->DrawImage(m_texture, dest );
		}
	}
}


void CHeightMapPanel::OnBnClickedOk()
{
}


void CHeightMapPanel::OnBnClickedCancel()
{
}


void CHeightMapPanel::UpdateTerrainInfo()
{
	m_heightFactor = cMapController::Get()->GetTerrain().GetHeightFactor();
	m_heightSlider.SetPos( (int)(m_heightFactor * 100.f) );

	m_uvFactor = cMapController::Get()->GetTerrain().GetTextureUVFactor();
	m_uvSlider.SetPos( (int)(m_uvFactor * 100.f) );

	UpdateData(FALSE);
}


void CHeightMapPanel::UpdateHeightMapList()
{
	// 파일 찾기.
	list<string> extList;
	extList.push_back("jpg");
	extList.push_back("png");
	extList.push_back("bmp");

	m_heightmapTree.Update( "../media/terrain/", extList, "");
	m_heightmapTree.ExpandAll();
}


// 지형 텍스쳐 리스트 출력.
void CHeightMapPanel::UpdateTextureList()
{
	// 파일 찾기.
	list<string> extList;
	extList.push_back("jpg");
	extList.push_back("png");
	extList.push_back("bmp");

	m_textureTree.Update( "../media/terrain/", extList, "");
	m_textureTree.ExpandAll();
}


// MapController 가 업데이트 될 때 호출된다.
void CHeightMapPanel::Update(int type)
{
	switch (type)
	{
	case NOTIFY_TYPE::NOTIFY_CHANGE_TERRAIN:
		UpdateTerrainInfo();
		break;
	}
}


void CHeightMapPanel::OnNMCustomdrawSliderHeightFactor(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_heightFactor = m_heightSlider.GetPos() / 100.f;
	cMapController::Get()->UpdateHeightFactor(m_heightFactor);
	UpdateData(FALSE);
	*pResult = 0;
}


void CHeightMapPanel::OnEnChangeEditHeightFactor()
{
	UpdateData();
	m_heightSlider.SetPos( (int)(m_heightFactor * 100.f) );
	cMapController::Get()->UpdateHeightFactor(m_heightFactor);
}


void CHeightMapPanel::OnNMCustomdrawSliderUvFactor(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_uvFactor = m_uvSlider.GetPos() / 100.f;
	cMapController::Get()->GetTerrain().SetTextureUVFactor(m_uvFactor);
	UpdateData(FALSE);
	*pResult = 0;
}


void CHeightMapPanel::OnEnChangeEditUvFactor()
{
	UpdateData();
	m_uvSlider.SetPos( (int)(m_uvFactor * 100.f) );
	cMapController::Get()->GetTerrain().SetTextureUVFactor(m_uvFactor);
}


void CHeightMapPanel::OnBnClickedButtonRefresh()
{
	UpdateHeightMapList();
	UpdateTextureList();	
}


void CHeightMapPanel::OnTvnSelchangedTreeHeightmap(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_heightmapTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == ".") || (fileName == ".."))
		return;

	SAFE_DELETE(m_heightMap);
	m_heightMap = Image::FromFile(str2wstr(fileName).c_str());
	InvalidateRect(NULL, FALSE);

	cMapController::Get()->LoadHeightMap(*g_renderer, fileName);
	m_heightmapBrowser.SetWindowText(str2wstr(fileName).c_str());
}


void CHeightMapPanel::OnTvnSelchangedTreeTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_textureTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media") || (fileName == ".") || (fileName == ".."))
		return;

	SAFE_DELETE(m_texture);
	m_texture = Image::FromFile(str2wstr(fileName).c_str());
	InvalidateRect(NULL, FALSE);

	cMapController::Get()->LoadHeightMapTexture(*g_renderer, fileName);
	m_textureBrowser.SetWindowText(str2wstr(fileName).c_str());
}


void CHeightMapPanel::OnEnChangeMfceditbrowseHeightmap()
{
	CString wFileName;
	m_heightmapBrowser.GetWindowText(wFileName);

	const string fileName = wstr2str((wstring)wFileName);

	SAFE_DELETE(m_heightMap);
	m_heightMap = Image::FromFile(str2wstr(fileName).c_str());
	InvalidateRect(NULL, FALSE);

	cMapController::Get()->LoadHeightMap(*g_renderer, fileName);
}


void CHeightMapPanel::OnEnChangeMfceditbrowseTexture()
{
	CString wFileName;
	m_textureBrowser.GetWindowText(wFileName);

	const string fileName = wstr2str((wstring)wFileName);

	SAFE_DELETE(m_texture);
	m_texture = Image::FromFile(str2wstr(fileName).c_str());
	InvalidateRect(NULL, FALSE);

	cMapController::Get()->LoadHeightMapTexture(*g_renderer, fileName);
}


void CHeightMapPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(m_heightmapTree, cx, cy);
	MoveChildCtrlWindow(m_textureTree, cx, cy);
	MoveChildCtrlWindow(m_heightmapBrowser, cx-90, cy);
	MoveChildCtrlWindow(m_textureBrowser, cx-90, cy);
}
