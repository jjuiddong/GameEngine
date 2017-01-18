// AnimationPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "AnimationPanel.h"

using namespace graphic;

// CAnimationPanel 대화 상자입니다.

CAnimationPanel::CAnimationPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CAnimationPanel::IDD, pParent)
	, m_FilePath(_T(""))
	, m_StartFrame(0)
	, m_EndFrame(0)
	, m_FileName(_T(""))
{

}

CAnimationPanel::~CAnimationPanel()
{
}

void CAnimationPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_FilePath);
	DDX_Text(pDX, IDC_EDIT_START_TIME, m_StartFrame);
	DDX_Text(pDX, IDC_EDIT_END_TIME, m_EndFrame);
	DDX_Control(pDX, IDC_TREE_ANI, m_AniTree);
	DDX_Text(pDX, IDC_STATIC_FILENAME2, m_FileName);
}


BEGIN_MESSAGE_MAP(CAnimationPanel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CAnimationPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAnimationPanel::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PANEL_DETAILANIMATIONINFO, &CAnimationPanel::OnPanelDetailanimationinfo)
END_MESSAGE_MAP()


// CAnimationPanel 메시지 처리기입니다.


void CAnimationPanel::OnBnClickedOk()
{
}


void CAnimationPanel::OnBnClickedCancel()
{
}


void CAnimationPanel::Update(int type)
{
	if (NOTIFY_MSG::UPDATE_MODEL == type)
	{
		UpdateAnimationInfo();
		UpdateAnimationTree();
		UpdateData(FALSE);
	}
}


void CAnimationPanel::UpdateAnimationInfo()
{
	const sRawAniGroup *rawAnies= cResourceManager::Get()->LoadAnimation( 
		cController::Get()->GetCurrentAnimationFileName() );
	RET (!rawAnies);

	 string filePath = cController::Get()->GetCurrentAnimationFileName();
	 m_FilePath = filePath.c_str();
	 m_FileName =common::GetFileName(filePath).c_str();
	RET (rawAnies->anies.empty());	

	m_StartFrame = rawAnies->anies[ 0].start;
	m_EndFrame = rawAnies->anies[ 0].end;
}


// 애니메이션 정보 출력.
void CAnimationPanel::UpdateAnimationTree(bool showDetails)// showDetails=false
{
	m_AniTree.DeleteAllItems();

	const sRawAniGroup *rawAnies= cResourceManager::Get()->LoadAnimation( 
		cController::Get()->GetCurrentAnimationFileName() );
	RET (!rawAnies);

	const wstring rootStr = formatw( "Animation [%d]", rawAnies->anies.size());
	const HTREEITEM hRoot = m_AniTree.InsertItem(rootStr.c_str());

	for (u_int i=0; i < rawAnies->anies.size(); ++i)
	{
		const sRawAni &ani = rawAnies->anies[ i];
		const wstring str = ani.name.empty()? formatw("track%d", i+1) : str2wstr(ani.name);
		const HTREEITEM hItem = m_AniTree.InsertItem( str.c_str(), hRoot);
		InsertAnimationInfo(hItem, ani);

		if (showDetails && !rawAnies->bones.empty())
			InsertAnimationDetailInfoBone(hItem, rawAnies->bones[ i]);
		if (showDetails)
			InsertAnimationDetailInfoAni(hItem, ani);
	}

	m_AniTree.Expand(hRoot, TVE_EXPAND);
}


// 애니메이션 정보를 트리에 추가한다.
void CAnimationPanel::InsertAnimationInfo(HTREEITEM hItem, const sRawAni &ani)
{
	const wstring name = formatw("name = %s", ani.name.c_str());
	const wstring start = formatw("start = %d", (int)ani.start);
	const wstring end = formatw("end = %d", (int)ani.end);
	const wstring keypos = formatw("keyPos Count = %d", ani.pos.size());
	const wstring keyrot = formatw("keyRot Count = %d", ani.rot.size());
	const wstring keyscale = formatw("keyScale Count = %d", ani.scale.size());

	m_AniTree.InsertItem( name.c_str(), hItem);
	m_AniTree.InsertItem( start.c_str(), hItem);
	m_AniTree.InsertItem( end.c_str(), hItem);
	m_AniTree.InsertItem( keypos.c_str(), hItem);
	m_AniTree.InsertItem( keyrot.c_str(), hItem);
	m_AniTree.InsertItem( keyscale.c_str(), hItem);
}


// 애니메이션의 이동, 회전, 스케일링 정보를 출력한다.
void CAnimationPanel::InsertAnimationDetailInfoBone(HTREEITEM hItem, const graphic::sRawBone &bone)
{
	// local tm
	if (HTREEITEM hLocal = m_AniTree.InsertItem( L"localTM", hItem))
	{
		const wstring mat1 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.localTm._11, bone.localTm._12, bone.localTm._13, bone.localTm._14 );
		const wstring mat2 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.localTm._21, bone.localTm._22, bone.localTm._23, bone.localTm._24 );
		const wstring mat3 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.localTm._31, bone.localTm._32, bone.localTm._33, bone.localTm._34 );
		const wstring mat4 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.localTm._41, bone.localTm._42, bone.localTm._43, bone.localTm._44 );

		m_AniTree.InsertItem( mat1.c_str(), hLocal);
		m_AniTree.InsertItem( mat2.c_str(), hLocal);
		m_AniTree.InsertItem( mat3.c_str(), hLocal);
		m_AniTree.InsertItem( mat4.c_str(), hLocal);
	}


	// world tm
	if (HTREEITEM hWorld = m_AniTree.InsertItem( L"worldTM", hItem))
	{
		const wstring mat1 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.worldTm._11, bone.worldTm._12, bone.worldTm._13, bone.worldTm._14 );
		const wstring mat2 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.worldTm._21, bone.worldTm._22, bone.worldTm._23, bone.worldTm._24 );
		const wstring mat3 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.worldTm._31, bone.worldTm._32, bone.worldTm._33, bone.worldTm._34 );
		const wstring mat4 = formatw("%.3f, %.3f, %.3f, %.3f", 
			bone.worldTm._41, bone.worldTm._42, bone.worldTm._43, bone.worldTm._44 );

		m_AniTree.InsertItem( mat1.c_str(), hWorld);
		m_AniTree.InsertItem( mat2.c_str(), hWorld);
		m_AniTree.InsertItem( mat3.c_str(), hWorld);
		m_AniTree.InsertItem( mat4.c_str(), hWorld);
	}

}


// 애니메이션의 이동, 회전, 스케일링 정보를 출력한다.
void CAnimationPanel::InsertAnimationDetailInfoAni(HTREEITEM hItem, const graphic::sRawAni &ani )
{

	// key pos
	if (HTREEITEM hKeyPos = m_AniTree.InsertItem( 
		formatw("keyPos Animation [%d]", ani.pos.size()).c_str(), hItem))
	{
		for (u_int i=0; i < ani.pos.size(); ++i)
		{
			const wstring str = formatw("frame %.0f = {%.3f, %.3f, %.3f}", ani.pos[ i].t, 
				ani.pos[ i].p.x, ani.pos[ i].p.y, ani.pos[ i].p.z);

			m_AniTree.InsertItem( str.c_str(), hKeyPos);
		}
	}


	// key rot
	if (HTREEITEM hKeyRot = m_AniTree.InsertItem( 
		formatw("keyRot Animation [%d]", ani.rot.size()).c_str(), hItem))
	{
		for (u_int i=0; i < ani.rot.size(); ++i)
		{
			const wstring str = formatw("frame %.0f = {%.3f, %.3f, %.3f, %.3f}", ani.rot[ i].t, 
				ani.rot[ i].q.x, ani.rot[ i].q.y, ani.rot[ i].q.z, ani.rot[ i].q.w);

			m_AniTree.InsertItem( str.c_str(), hKeyRot);
		}
	}


	// key scale
	if (HTREEITEM hKeyScale = m_AniTree.InsertItem( 
		formatw("keyScale Animation [%d]", ani.scale.size()).c_str(), hItem))
	{
		for (u_int i=0; i < ani.scale.size(); ++i)
		{
			const wstring str = formatw("frame %.0f = {%.3f, %.3f, %.3f}", ani.scale[ i].t, 
				ani.scale[ i].s.x, ani.scale[ i].s.y, ani.scale[ i].s.z);

			m_AniTree.InsertItem( str.c_str(), hKeyScale);
		}
	}

}


void CAnimationPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
}


void CAnimationPanel::OnContextMenu(CWnd* pWnd, CPoint /*point*/)
{
	if (&m_AniTree == pWnd)
	{
		CPoint p;
		GetCursorPos(&p);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_PANEL_DETAILANIMATIONINFO, _T("Detail Animation Info"));
		menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	}
}


// 애니메이션 세부정보 출력.
void CAnimationPanel::OnPanelDetailanimationinfo()
{
	UpdateAnimationTree(true);
}
