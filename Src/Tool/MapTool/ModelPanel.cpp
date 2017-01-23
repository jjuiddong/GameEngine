// ModelPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "ModelPanel.h"
#include "afxdialogex.h"
#include "MapView.h"

using namespace graphic;


// CModelPanel 대화 상자입니다.
CModelPanel::CModelPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CModelPanel::IDD, pParent)
	, m_PosX(0)
,m_PosY(0)
,m_PosZ(0)
,m_RotX(0)
,m_RotY(0)
,m_RotZ(0)
,m_ScaleX(0)
,m_ScaleY(0)
,m_ScaleZ(0)
, m_modelName(_T(""))
{

}

CModelPanel::~CModelPanel()
{
}

void CModelPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PLACE_MODEL, m_placeModelList);
	DDX_Control(pDX, IDC_TREE_MODEL, m_modelTree);
	DDX_Text(pDX, IDC_EDIT_POSX, m_PosX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_PosY);
	DDX_Text(pDX, IDC_EDIT_POSZ, m_PosZ);
	DDX_Text(pDX, IDC_EDIT_ROTX, m_RotX);
	DDX_Text(pDX, IDC_EDIT_ROTY, m_RotY);
	DDX_Text(pDX, IDC_EDIT_ROTZ, m_RotZ);
	DDX_Text(pDX, IDC_EDIT_SCALEX, m_ScaleX);
	DDX_Text(pDX, IDC_EDIT_SCALEY, m_ScaleY);
	DDX_Text(pDX, IDC_EDIT_SCALEZ, m_ScaleZ);
	DDX_Text(pDX, IDC_STATIC_MODEL_NAME, m_modelName);
}


BEGIN_MESSAGE_MAP(CModelPanel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CModelPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModelPanel::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CModelPanel::OnBnClickedButtonRefresh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MODEL, &CModelPanel::OnTvnSelchangedTreeModel)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MODELMENU_DELETEMODEL, &CModelPanel::OnModelmenuDeletemodel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PLACE_MODEL, &CModelPanel::OnLvnItemchangedListPlaceModel)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_POSX, IDC_EDIT_SCALEZ, &CModelPanel::OnEnChangeEditModel)
END_MESSAGE_MAP()


// CModelPanel 메시지 처리기입니다.
void CModelPanel::OnBnClickedOk()
{
}
void CModelPanel::OnBnClickedCancel()
{
}


BOOL CModelPanel::OnInitDialog()
{
	__super::OnInitDialog();

	m_placeModelList.InsertColumn(0, L"num", LVCFMT_LEFT, 35);
	m_placeModelList.InsertColumn(1, L"model", LVCFMT_LEFT, 300);
	m_placeModelList.SetExtendedStyle(m_placeModelList.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT);

	UpdateModelList("../media/" );

	return TRUE;
}


void CModelPanel::Update(int type)
{
	switch (type)
	{
	case NOTIFY_TYPE::NOTIFY_ADD_PLACE_MODEL:
		UpdatePlaceModelList();
		break;

	case NOTIFY_TYPE::NOTIFY_SELECT_PLACE_MODEL:
		{
			if (const cModel *model = g_mapView->GetFocusModel())
			{
				UpdateModelInfo(*model);
			}
		}
		break;
	}
}


void CModelPanel::UpdateModelList(const string &directoryPath)
{
	// 파일 찾기.
	list<string> extList;
	extList.push_back("dat");

	m_modelTree.Update( directoryPath, extList);
}


// 지형에 위치한 모델들을 리스트에 출력한다.
void CModelPanel::UpdatePlaceModelList()
{
	m_placeModelList.DeleteAllItems();

	vector<graphic::cModel*> &models = cMapController::Get()->GetTerrain().GetRigidModels();
	for (u_int i=0; i < models.size(); ++i)
	{
		const wstring num = common::formatw("%d", i+1);
		const wstring fileName = str2wstr( common::GetFileName(models[ i]->GetFileName()) );

		m_placeModelList.InsertItem(i, num.c_str());
		m_placeModelList.SetItemText(i, 1, fileName.c_str());
		m_placeModelList.SetItemData(i, models[ i]->GetId() );
	}
}


// 모델 정보를 화면에 표시한다.
void CModelPanel::UpdateModelInfo(const cModel &model, const bool updateList)//updateList=true
{
	// 리스트에서 해당 아이템을 활성화 한다.
	LVFINDINFO info;
	info.flags = LVFI_PARAM;
	info.lParam = model.GetId();
	const int index = m_placeModelList.FindItem(&info);
	if (updateList && index >= 0)
	{
		// 현재 리스트에서 강조중인 아이템을 초기화 한다.
		for (int i=0; i < m_placeModelList.GetItemCount(); ++i)
			m_placeModelList.SetItemState(i, 0, 0xFF);

		// 선택된 모델 강조.
		m_placeModelList.EnsureVisible(index, FALSE);
		m_placeModelList.SetSelectionMark(index);
		m_placeModelList.SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, 0xFF);
		m_placeModelList.UpdateWindow();
	}

	// 모델 정보 업데이트
	wstring modelName = str2wstr(model.GetName());
	m_modelName = (LPCTSTR)modelName.c_str();

	//const Vector3 pos = model.GetTransform().GetPosition();
	//const Quaternion q = model.GetTransform().GetQuaternion();
	//Vector3 rot = q.Euler();
	//rot = Vector3(RAD2ANGLE(rot.x), RAD2ANGLE(rot.y), RAD2ANGLE(rot.z));

	//Matrix44 m = model.GetTransform();
	//m = m * q.GetMatrix().Inverse();
	//const Vector3 scale = m.GetScale();

	m_PosX = model.GetToolTransform().pos.x;
	m_PosY = model.GetToolTransform().pos.y;
	m_PosZ = model.GetToolTransform().pos.z;
	m_RotX = model.GetToolTransform().rot.x;
	m_RotY = model.GetToolTransform().rot.y;
	m_RotZ = model.GetToolTransform().rot.z;
	m_ScaleX = model.GetToolTransform().scale.x;
	m_ScaleY = model.GetToolTransform().scale.y;
	m_ScaleZ = model.GetToolTransform().scale.z;

	UpdateData(FALSE);
}


void CModelPanel::OnBnClickedButtonRefresh()
{
	UpdateModelList("../media/" );
}


void CModelPanel::OnTvnSelchangedTreeModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const string fileName = m_modelTree.GetSelectFilePath(pNMTreeView->itemNew.hItem);
	if (common::GetFileExt(fileName).empty() || (fileName == "./media"))
		return;

	ShowLoadingDialog();
	cMapController::Get()->GetTerrainCursor().SelectModel( fileName);
	HideLoadingDialog();
}


void CModelPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(m_modelTree, cx, cy);
	MoveChildCtrlWindow(m_placeModelList, cx, cy);
}


void CModelPanel::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (&m_placeModelList == pWnd)
	{
		if (m_placeModelList.GetSelectedCount() <= 0)
			return;

		CPoint p;
		GetCursorPos(&p);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_MODELMENU_DELETEMODEL, _T("Delete Model"));
		menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);		
	}

}


void CModelPanel::OnModelmenuDeletemodel()
{
	 const int idx = m_placeModelList.GetSelectionMark();
	 if (idx >= 0)
	 {
		 const int modelId = m_placeModelList.GetItemData(idx);
		 cMapController::Get()->GetTerrain().RemoveRigidModel(modelId);
		UpdatePlaceModelList();
	 }
}


void CModelPanel::OnLvnItemchangedListPlaceModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uChanged & LVIF_STATE) 
		&& (pNMLV->uNewState & LVNI_SELECTED))
	{
		const int modelId =m_placeModelList.GetItemData(pNMLV->iItem);
		 if (cModel *model = cMapController::Get()->GetTerrain().FindRigidModel(modelId))
		 {
			 vector<graphic::cModel*> &models = cMapController::Get()->GetTerrain().GetRigidModels();
			 for (u_int i=0; i < models.size(); ++i)
				models[ i]->SetRenderBoundingBox(false);

			 UpdateModelInfo(*model, false);
			 model->SetRenderBoundingBox( !model->IsRenderBoundingBox() );
			 g_mapView->SetFocusModel(model);
		 }
	}
}


void CModelPanel::OnEnChangeEditModel(UINT id)
{
	if (cModel *model = g_mapView->GetFocusModel())
	{
		UpdateData();

		sTransform tm;
		tm.pos = Vector3(m_PosX, m_PosY, m_PosZ);
		tm.rot = Vector3(m_RotX, m_RotY, m_RotZ);
		tm.scale = Vector3(m_ScaleX, m_ScaleY, m_ScaleZ);
		model->SetToolTransform(tm);
	}
}
