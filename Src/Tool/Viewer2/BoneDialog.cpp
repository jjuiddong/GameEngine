// BoneDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "BoneDialog.h"
#include "Viewer2Dlg.h"
#include "MainPanel.h"


CBoneDialog *g_boneDlg = NULL;

// CBoneDialog 대화 상자입니다.
CBoneDialog::CBoneDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBoneDialog::IDD, pParent)
	, m_boneCount(0)
{

}

CBoneDialog::~CBoneDialog()
{
}

void CBoneDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_BONE, m_boneTree);
	DDX_Control(pDX, IDC_TREE_BONENODE, m_nodeInfoTree);
	DDX_Text(pDX, IDC_STATIC_BONECOUNT, m_boneCount);
}


BEGIN_MESSAGE_MAP(CBoneDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CBoneDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBoneDialog::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BONE, &CBoneDialog::OnSelchangedTreeBone)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CBoneDialog 메시지 처리기입니다.


void CBoneDialog::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}


void CBoneDialog::OnBnClickedCancel()
{
	HideBoneDialog();
}


void CBoneDialog::Update(int type)
{
	if (NOTIFY_MSG::UPDATE_MODEL == type)
	{
		UpdateTree();
	}
}


bool CBoneDialog::UpdateTree()
{
	graphic::cCharacter *character = cController::Get()->GetCharacter();
	RETV(!character, false);

	graphic::cBoneMgr *boneMgr = character->GetBoneMgr();
	RETV(!boneMgr, false);

	graphic::cBoneNode *root = boneMgr->GetRoot();
	RETV(!root, false);

	m_boneTree.Update( boneMgr );
	m_boneTree.ExpandAll();

	// Bones Tree, bone root 제외한 아이템 개수가 본 개수이다.
	m_boneCount = max(m_boneTree.GetCount()-2, 0);
	UpdateData(FALSE);

	return true;
}


void CBoneDialog::OnSelchangedTreeBone(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	graphic::cCharacter *character = cController::Get()->GetCharacter();
	RET(!character);

	graphic::cBoneMgr *boneMgr = character->GetBoneMgr();
	RET(!boneMgr);

	// 선택한 노드의 본을 강조시킨다.
	CString str = m_boneTree.GetItemText(pNMTreeView->itemNew.hItem);
	graphic::cBoneNode *boneNode = boneMgr->FindBone(wstr2str((wstring)str));
	RET(!boneNode);

	UpdateBoneNodeInfo(boneNode);

	*pResult = 0;
}


void CBoneDialog::UpdateBoneNodeInfo(graphic::cBoneNode *boneNode)
{
	RET(!boneNode);

	m_nodeInfoTree.DeleteAllItems();

	HTREEITEM hItem = m_nodeInfoTree.InsertItem( 
		str2wstr(boneNode->GetName()).c_str(), NULL );

	Matrix44 localTm = boneNode->GetLocalTM();
	Matrix44 worldTm = boneNode->GetOffset().Inverse();

	// local tm
	if (HTREEITEM hLocal = m_nodeInfoTree.InsertItem( L"localTM", hItem))
	{
		const wstring mat1 = formatw("%.3f, %.3f, %.3f, %.3f", 
			localTm._11, localTm._12, localTm._13, localTm._14 );
		const wstring mat2 = formatw("%.3f, %.3f, %.3f, %.3f", 
			localTm._21, localTm._22, localTm._23, localTm._24 );
		const wstring mat3 = formatw("%.3f, %.3f, %.3f, %.3f", 
			localTm._31, localTm._32, localTm._33, localTm._34 );
		const wstring mat4 = formatw("%.3f, %.3f, %.3f, %.3f", 
			localTm._41, localTm._42, localTm._43, localTm._44 );

		m_nodeInfoTree.InsertItem( mat1.c_str(), hLocal);
		m_nodeInfoTree.InsertItem( mat2.c_str(), hLocal);
		m_nodeInfoTree.InsertItem( mat3.c_str(), hLocal);
		m_nodeInfoTree.InsertItem( mat4.c_str(), hLocal);
	}


	// world tm
	if (HTREEITEM hWorld = m_nodeInfoTree.InsertItem( L"worldTM", hItem))
	{
		const wstring mat1 = formatw("%.3f, %.3f, %.3f, %.3f", 
			worldTm._11, worldTm._12, worldTm._13, worldTm._14 );
		const wstring mat2 = formatw("%.3f, %.3f, %.3f, %.3f", 
			worldTm._21, worldTm._22, worldTm._23, worldTm._24 );
		const wstring mat3 = formatw("%.3f, %.3f, %.3f, %.3f", 
			worldTm._31, worldTm._32, worldTm._33, worldTm._34 );
		const wstring mat4 = formatw("%.3f, %.3f, %.3f, %.3f", 
			worldTm._41, worldTm._42, worldTm._43, worldTm._44 );

		m_nodeInfoTree.InsertItem( mat1.c_str(), hWorld);
		m_nodeInfoTree.InsertItem( mat2.c_str(), hWorld);
		m_nodeInfoTree.InsertItem( mat3.c_str(), hWorld);
		m_nodeInfoTree.InsertItem( mat4.c_str(), hWorld);
	}

	ExpandAll(m_nodeInfoTree);
}


void CBoneDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(*this, m_boneTree, cx, cy);
	MoveChildCtrlWindow(*this, m_nodeInfoTree, cx, cy);
}


void InitBoneDialog(CWnd *parent)
{
	g_boneDlg = new CBoneDialog(parent);
	g_boneDlg->Create(CBoneDialog::IDD, parent);

	cController::Get()->AddObserver(g_boneDlg);
}


void ShowBoneDialog()
{
	if (g_boneDlg)
	{
		g_boneDlg->UpdateTree();

		// 부모 윈도우의 옆에 출력하게 한다.
		CRect pr, wr;
		CMainPanel *dlg = cController::Get()->GetViewerDlg()->GetMainPanel();
		dlg->GetWindowRect(pr);
		g_boneDlg->GetWindowRect(wr);

		const int x = pr.left;
		const int y = pr.top;
		g_boneDlg->MoveWindow(x, y, wr.Width(), wr.Height());

		g_boneDlg->ShowWindow(SW_SHOW);
	}
}


void HideBoneDialog()
{
	if (g_boneDlg)
	{
		g_boneDlg->ShowWindow(SW_HIDE);
	}
}


void DestroyBoneDialog()
{
	SAFE_DELETE(g_boneDlg);
}
