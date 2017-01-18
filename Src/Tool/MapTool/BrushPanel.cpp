// BrushPanel.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "BrushPanel.h"
#include "afxdialogex.h"
#include <afxwin.h>


// CBrushPanel ��ȭ �����Դϴ�.
CBrushPanel::CBrushPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CBrushPanel::IDD, pParent)
,	m_texture(NULL)
, m_innerRadius(0)
, m_outerRadius(0)
, m_IsEraseMode(FALSE)
{

}

CBrushPanel::~CBrushPanel()
{
}

void CBrushPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_TEXTURE, m_textureBrowser);
	DDX_Control(pDX, IDC_LIST_LAYER, m_layerList);
	DDX_Control(pDX, IDC_SLIDER_INNER_RADIUS, m_innerRSlider);
	DDX_Control(pDX, IDC_SLIDER_OUTER_RADIUS, m_outerRSlider);
	DDX_Text(pDX, IDC_EDIT_INNER_RADIUS, m_innerRadius);
	DDX_Text(pDX, IDC_EDIT_OUTER_RADIUS2, m_outerRadius);
	DDX_Check(pDX, IDC_CHECK_ERASE, m_IsEraseMode);
	DDX_Control(pDX, IDC_TREE_BRUSH, m_brushTree);
}


BEGIN_MESSAGE_MAP(CBrushPanel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CBrushPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBrushPanel::OnBnClickedCancel)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_TEXTURE, &CBrushPanel::OnChangeMfceditbrowseTexture)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_INNER_RADIUS, &CBrushPanel::OnNMCustomdrawSliderInnerRadius)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_OUTER_RADIUS, &CBrushPanel::OnNMCustomdrawSliderOuterRadius)
	ON_EN_CHANGE(IDC_EDIT_INNER_RADIUS, &CBrushPanel::OnEnChangeEditInnerRadius)
	ON_EN_CHANGE(IDC_EDIT_OUTER_RADIUS2, &CBrushPanel::OnEnChangeEditOuterRadius2)
	ON_BN_CLICKED(IDC_CHECK_ERASE, &CBrushPanel::OnBnClickedCheckErase)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_BRUSHMENU_DELETE_LAYER, &CBrushPanel::OnDeleteLayer)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CBrushPanel::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BRUSH, &CBrushPanel::OnTvnSelchangedTreeBrush)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CBrushPanel �޽��� ó�����Դϴ�.
BOOL CBrushPanel::OnInitDialog()
{
	__super::OnInitDialog();

	m_layerList.InsertColumn(0, L"Layer", LVCFMT_LEFT, 40 );
	m_layerList.InsertColumn(1, L"Texture", LVCFMT_LEFT, 300 );
	m_layerList.SetExtendedStyle(m_layerList.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT);

	m_textureBrowser.EnableFileBrowseButton(_T("Texture"), 
		_T("Image files|*.jpg;*.png;*.bmp|All files|*.*||"));

	UpdateTextureFiles("./media/terrain/");

	graphic::cTerrainCursor &cursor = cMapController::Get()->GetTerrainCursor();
	m_innerRadius = cursor.GetInnerBrushRadius();
	m_outerRadius = cursor.GetOuterBrushRadius();

	m_innerRSlider.SetRange(0, 300);
	m_outerRSlider.SetRange(0, 300);

	m_innerRSlider.SetPos( cursor.GetInnerBrushRadius() );
	m_outerRSlider.SetPos( cursor.GetOuterBrushRadius() );

	UpdateData(FALSE);

	return TRUE;
}


void CBrushPanel::OnBnClickedOk()
{
}


void CBrushPanel::OnBnClickedCancel()
{
}


void CBrushPanel::Update(int type)
{
	switch (type)
	{
	case NOTIFY_TYPE::NOTIFY_CHANGE_TERRAIN:
	case NOTIFY_TYPE::NOTIFY_ADD_LAYER:
		UpdateLayerList();
		m_IsEraseMode = FALSE;
		cMapController::Get()->GetTerrainCursor().EnableEraseMode(false);
		break;

	case NOTIFY_TYPE::NOTIFY_CHANGE_SPLATLAYER:
		UpdateLayerList();
		break;
	}
}


void CBrushPanel::UpdateTextureFiles(const string &directoryPath)
{
	// ���� ã��.
	list<string> extList;
	extList.push_back("jpg");
	extList.push_back("png");
	extList.push_back("bmp");

	m_brushTree.Update( directoryPath, extList);
	m_brushTree.ExpandAll();
}


void CBrushPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Graphics *graph = Graphics::FromHDC(dc.GetSafeHdc());

	if (m_texture)
	{
		// �ؽ��� ���.
		if (CWnd *pictureCtlr = GetDlgItem(IDC_STATIC_TEXTURE))
		{
			CRect cr;
			pictureCtlr->GetWindowRect(cr);
			ScreenToClient(cr);

			const Rect dest(cr.left, cr.top, cr.Width(), cr.Height());
			graph->DrawImage(m_texture, dest );
		}
	}
}


void CBrushPanel::OnChangeMfceditbrowseTexture()
{
	CString wfileName;
	m_textureBrowser.GetWindowText(wfileName);
	string fileName = common::wstr2str((wstring)wfileName);

	SAFE_DELETE(m_texture);
	m_texture = Image::FromFile(wfileName);

	cMapController::Get()->GetTerrainCursor().SelectBrushTexture(fileName);

	InvalidateRect(NULL, FALSE);
}


// ���� ���̾� ����Ʈ�� ������Ʈ �Ѵ�.
void CBrushPanel::UpdateLayerList()
{
	m_layerList.DeleteAllItems();

	graphic::cTerrainEditor &terrain = cMapController::Get()->GetTerrain();
	for (int i=0; i < terrain.GetLayerCount(); ++i)
	{
		wstring layer = common::formatw( "%d", i + 1 );
		wstring texture = str2wstr(terrain.GetLayer(i).texture->GetTextureName());
		m_layerList.InsertItem( i, layer.c_str() );
		m_layerList.SetItemText(i, 1, texture.c_str());
	}

}


void CBrushPanel::OnNMCustomdrawSliderInnerRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	m_innerRadius = m_innerRSlider.GetPos();
	cMapController::Get()->GetTerrainCursor().SetInnerBrushRadius(m_innerRadius);
	cMapController::Get()->UpdateBrush();
	UpdateData(FALSE);
	*pResult = 0;
}


void CBrushPanel::OnNMCustomdrawSliderOuterRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	m_outerRadius = m_outerRSlider.GetPos();
	cMapController::Get()->GetTerrainCursor().SetOuterBrushRadius(m_outerRadius);
	cMapController::Get()->UpdateBrush();
	UpdateData(FALSE);
	*pResult = 0;
}


void CBrushPanel::OnEnChangeEditInnerRadius()
{
	UpdateData();
	m_innerRSlider.SetPos(m_innerRadius);
	cMapController::Get()->GetTerrainCursor().SetInnerBrushRadius(m_innerRadius);
	cMapController::Get()->UpdateBrush();
}


void CBrushPanel::OnEnChangeEditOuterRadius2()
{
	UpdateData();
	m_outerRSlider.SetPos(m_outerRadius);
	cMapController::Get()->GetTerrainCursor().SetOuterBrushRadius(m_outerRadius);
	cMapController::Get()->UpdateBrush();
}


void CBrushPanel::OnBnClickedCheckErase()
{
	UpdateData();
	cMapController::Get()->GetTerrainCursor().EnableEraseMode(m_IsEraseMode? true : false);
}


// �޴� ���.
void CBrushPanel::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (&m_layerList == pWnd)
	{
		if (m_layerList.GetSelectedCount() <= 0)
			return;

		CPoint p;
		GetCursorPos(&p);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_BRUSHMENU_DELETE_LAYER, _T("Delete Layer"));
		menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);		
	}
}


// ���õ� ���̾� ����.
void CBrushPanel::OnDeleteLayer()
{
	if (m_layerList.GetSelectedCount() <= 0)
		return;

	POSITION pos = m_layerList.GetFirstSelectedItemPosition();
	const int item = m_layerList.GetNextSelectedItem(pos);
	if (item < 0)
		return;

	cMapController::Get()->GetTerrain().DeleteLayer(item);
	cMapController::Get()->UpdateSplatLayer();
}


void CBrushPanel::OnBnClickedButtonRefresh()
{
	UpdateTextureFiles("./media/terrain/");
}


void CBrushPanel::OnTvnSelchangedTreeBrush(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_brushTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media"))
		return;

	SAFE_DELETE(m_texture);
	m_texture = Image::FromFile(str2wstr(fileName).c_str());

	cMapController::Get()->GetTerrainCursor().SelectBrushTexture(fileName);
	m_textureBrowser.SetWindowText(str2wstr(fileName).c_str());

	InvalidateRect(NULL, FALSE);
}


void CBrushPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(m_brushTree, cx, cy);
	MoveChildCtrlWindow(m_layerList, cx, cy);	
	MoveChildCtrlWindow(m_textureBrowser, cx-90, cy);	
}
