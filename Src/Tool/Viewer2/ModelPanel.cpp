// ModelPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "ModelPanel.h"
#include "BoneDialog.h"
#include "ModelView.h"


using namespace graphic;


// CModelPanel 대화 상자입니다.

CModelPanel::CModelPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CModelPanel::IDD, pParent)
	, m_FilePath(_T(""))
	, m_FileName(_T(""))
{

}

CModelPanel::~CModelPanel()
{
}

void CModelPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MESH, m_MeshTree);
	DDX_Control(pDX, IDC_TREE_MATERIAL, m_MaterialTree);
	DDX_Control(pDX, IDC_TREE_RAWBONE, m_RawBoneTree);
	DDX_Control(pDX, IDC_TREE_BONE, m_BoneTree);
	DDX_Text(pDX, IDC_STATIC_FILEPATH, m_FilePath);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_FileName);
}


BEGIN_MESSAGE_MAP(CModelPanel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CModelPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModelPanel::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SHOW_BONE_TREE, &CModelPanel::OnBnClickedButtonShowBoneTree)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PANEL_SHOWHIDE_MESH, &CModelPanel::OnPanelShowhideMesh)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MATERIAL, &CModelPanel::OnSelchangedMaterialTree)
END_MESSAGE_MAP()


// CModelPanel 메시지 처리기입니다.

void CModelPanel::Update(int type)
{
	if (NOTIFY_MSG::UPDATE_MODEL == type)
	{
		UpdateModelInfo();
		UpdateMaterialInfo();
		UpdateMeshInfo();	
		UpdateBoneInfo();
		UpdateRawBoneInfo();

		UpdateData(FALSE);
	}
}


void CModelPanel::UpdateModelInfo()
{
	string filePath = cController::Get()->GetCurrentMeshFileName();
	m_FilePath = filePath.c_str();
	m_FileName = common::GetFileName(filePath).c_str();
}


// 메쉬 정보 업데이트
void CModelPanel::UpdateMeshInfo()
{
	m_MeshTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	const wstring rootStr = formatw( "Mehses [%d]", rawMeshses->meshes.size());
	const HTREEITEM hRoot = m_MeshTree.InsertItem(rootStr.c_str());

	for (u_int i=0; i < rawMeshses->meshes.size(); ++i)
	{
		const sRawMesh &mesh = rawMeshses->meshes[ i];

		const string meshName = GetMeshTokenizeName(mesh.name);
		const wstring str = formatw("%s", meshName.c_str());
		const HTREEITEM hItem = m_MeshTree.InsertItem( str.c_str(), hRoot);
		m_MeshTree.SetItemData(hItem, i+1); // 메쉬 인덱스+1 값을 저장한다.

		const wstring name = formatw("Name = %s", mesh.name.c_str());
		const wstring materialId = formatw("MaterialIds [%d]", mesh.mtrlIds.size());
		const wstring vertexCount = formatw("Vertex Count = %d", mesh.vertices.size());
		const wstring faceCount = formatw("Face Count = %d", mesh.indices.size()/3);
		const wstring normalCount = formatw("Normal Count = %d", mesh.normals.size());
		const wstring tangentCount = formatw("Tangent Count = %d", mesh.tangent.size());
		const wstring binormalCount = formatw("Binormal Count = %d", mesh.binormal.size());
		const wstring textureCount = formatw("Texture UV Count = %d", mesh.tex.size());
		const wstring vertexWeightCount = formatw("Vertex Weight Count = %d", mesh.weights.size());



		m_MeshTree.InsertItem( name.c_str(), hItem);

		const HTREEITEM hMtrlItem = m_MeshTree.InsertItem( materialId.c_str(), hItem);
		for (u_int k=0; k < mesh.mtrlIds.size(); ++k)
		{
			const wstring id = formatw("Material Id = %d", mesh.mtrlIds[ k]);
			m_MeshTree.InsertItem( id.c_str(), hMtrlItem);
		}

		m_MeshTree.InsertItem( vertexCount.c_str(), hItem);
		m_MeshTree.InsertItem( faceCount.c_str(), hItem);
		m_MeshTree.InsertItem( normalCount.c_str(), hItem);
		m_MeshTree.InsertItem( tangentCount.c_str(), hItem);
		m_MeshTree.InsertItem( binormalCount.c_str(), hItem);
		m_MeshTree.InsertItem( textureCount.c_str(), hItem);
		m_MeshTree.InsertItem( vertexWeightCount.c_str(), hItem);
	}

	m_MeshTree.Expand(hRoot, TVE_EXPAND);
}


// 매터리얼 정보 업데이트
void CModelPanel::UpdateMaterialInfo()
{
	m_MaterialTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	const wstring rootStr = formatw( "Materials [%d]", rawMeshses->mtrls.size());
	const HTREEITEM hRoot = m_MaterialTree.InsertItem(rootStr.c_str());

	for (u_int i=0; i < rawMeshses->mtrls.size(); ++i )
	{
		const sMaterial &mtrl = rawMeshses->mtrls[ i];
		const wstring str = formatw("Material%d", i+1);		
		const HTREEITEM hItem = m_MaterialTree.InsertItem( str.c_str(), hRoot);

		const wstring ambient = formatw("Ambient %.3f, %.3f, %.3f, %.3f", 
			mtrl.ambient.x, mtrl.ambient.y, mtrl.ambient.z, mtrl.ambient.w );
		const wstring diffuse = formatw("Diffuse %.3f, %.3f, %.3f, %.3f", 
			mtrl.diffuse.x, mtrl.diffuse.y, mtrl.diffuse.z, mtrl.diffuse.w );
		const wstring specular = formatw("Specular %.3f, %.3f, %.3f, %.3f", 
			mtrl.specular.x, mtrl.specular.y, mtrl.specular.z, mtrl.specular.w );
		const wstring emissive = formatw("Emissive %.3f, %.3f, %.3f, %.3f", 
			mtrl.emissive.x, mtrl.emissive.y, mtrl.emissive.z, mtrl.emissive.w );
		const wstring power = formatw("Power = %.3f", mtrl.power);
		const wstring dirPath = formatw("directory path = %s", mtrl.directoryPath.c_str());
		const wstring texture = formatw("diffuse texture = %s", mtrl.texture.c_str());
		const wstring specularTexture = formatw("specular texture = %s", mtrl.specularMap.c_str());
		const wstring bumpTexture = formatw("bump texture = %s", mtrl.bumpMap.c_str());
		const wstring selfIllumTexture = formatw("selfIllum texture = %s", mtrl.selfIllumMap.c_str());

		m_MaterialTree.InsertItem( ambient.c_str(), hItem);
		m_MaterialTree.InsertItem( diffuse.c_str(), hItem);
		m_MaterialTree.InsertItem( specular.c_str(), hItem);
		m_MaterialTree.InsertItem( emissive.c_str(), hItem);
		m_MaterialTree.InsertItem( power.c_str(), hItem);
		m_MaterialTree.InsertItem( dirPath.c_str(), hItem);
		const HTREEITEM hTexture = m_MaterialTree.InsertItem( texture.c_str(), hItem);
		const HTREEITEM hSpecular = m_MaterialTree.InsertItem( specularTexture.c_str(), hItem);
		const HTREEITEM hBump = m_MaterialTree.InsertItem( bumpTexture.c_str(), hItem);
		const HTREEITEM hSelfIllum = m_MaterialTree.InsertItem( selfIllumTexture.c_str(), hItem);

		// material index 값을 저장한다.
		m_MaterialTree.SetItemData(hTexture, i);
		m_MaterialTree.SetItemData(hSpecular, i);
		m_MaterialTree.SetItemData(hBump, i);
		m_MaterialTree.SetItemData(hSelfIllum, i);
	}

	m_MaterialTree.Expand(hRoot, TVE_EXPAND);
}


// 본 정보 업데이트
void CModelPanel::UpdateRawBoneInfo()
{
	m_RawBoneTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	const wstring rootStr = formatw( "Raw Bones [%d]", rawMeshses->bones.size());
	const HTREEITEM hRoot = m_RawBoneTree.InsertItem(rootStr.c_str());

	for (u_int i=0; i < rawMeshses->bones.size(); ++i)
	{
		const sRawBone &bone = rawMeshses->bones[ i];
		const wstring str = formatw("Bone%d [%s]", i+1, bone.name.c_str());
		const HTREEITEM hItem = m_RawBoneTree.InsertItem( str.c_str(), hRoot);

		const wstring name = formatw("Name = %s", bone.name.c_str());
		const wstring boneId = formatw("Bone Id = %d", bone.id);
		const wstring parentId = formatw("Parent Id = %d", bone.parentId);

		m_MaterialTree.InsertItem( name.c_str(), hItem);
		m_MaterialTree.InsertItem( boneId.c_str(), hItem);
		m_MaterialTree.InsertItem( parentId.c_str(), hItem);
	}

	m_RawBoneTree.Expand(hRoot, TVE_EXPAND);
}


// 본 정보 업데이트
void CModelPanel::UpdateBoneInfo()
{
	m_BoneTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	graphic::cCharacter *character = cController::Get()->GetCharacter();
	RET(!character);

	graphic::cBoneMgr *boneMgr = character->GetBoneMgr();
	RET(!boneMgr);	

	graphic::cBoneNode *root = boneMgr->GetRoot();
	RET(!root);

	m_BoneTree.Update( boneMgr );
	m_BoneTree.ExpandAll();
}

void CModelPanel::OnBnClickedOk()
{
}


void CModelPanel::OnBnClickedCancel()
{
}


void CModelPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	
	MoveChildCtrlWindow(m_MaterialTree, cx, cy);
	MoveChildCtrlWindow(m_MeshTree, cx, cy);
	MoveChildCtrlWindow(m_BoneTree, cx, cy);
	MoveChildCtrlWindow(m_RawBoneTree, cx, cy);
}


void CModelPanel::OnBnClickedButtonShowBoneTree()
{
	ShowBoneDialog();	
}


void CModelPanel::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// 메쉬트리에서 메뉴를 열었을 때.
	if (pWnd == &m_MeshTree)
	{
		// 루트를 가르키면 메뉴를 띄우지 않는다.
		HTREEITEM hItem = m_MeshTree.GetSelectedItem();
		if (m_MeshTree.GetRootItem() == hItem)
			return;

		CPoint p;
		GetCursorPos(&p);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_PANEL_SHOWHIDE_MESH, _T("Show/Hide Mesh"));
		menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);	
	}	
}


void CModelPanel::OnPanelShowhideMesh()
{
	HTREEITEM hItem = m_MeshTree.GetSelectedItem();
	
	// 선택된 트리노드가 몇 번째 메쉬인지 검색한다.
	// 노드 데이타에 메쉬 인덱스 + 1 값이 저장되어 있다.
	// 0은 인덱스가 설정되지 않은 노드다. 찾을 때까지 부모로 올라간다.
	int meshIndex = -1;
	while (hItem != m_MeshTree.GetRootItem())
	{
		const DWORD data = m_MeshTree.GetItemData(hItem);
		if (data == 0)
		{
			hItem = m_MeshTree.GetParentItem(hItem);
		}
		else
		{
			meshIndex = data-1;
			break;
		}
	}

	if (meshIndex >= 0)
	{
		if (cMesh *mesh = cController::Get()->GetCharacter()->GetMesh(meshIndex))
		{
			const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
				cController::Get()->GetCurrentMeshFileName() );
			RET (!rawMeshses);

			const sRawMesh &rawMesh = rawMeshses->meshes[ meshIndex];
			const string meshName = GetMeshTokenizeName(rawMesh.name);
			wstring str = formatw("%s", meshName.c_str());
			if (mesh->IsRender())
				str += L" (Hide)";
			else
				str += L" (Show)";
			
			m_MeshTree.SetItemText(hItem, str.c_str());
			mesh->SetRender( !mesh->IsRender() );
		}
	}
}


// 메쉬 이름은 파일이름 + 메쉬 이름으로 구성되어 있다.
// 파일이름을 제외한 메쉬이름을 출력하는 코드다.
string CModelPanel::GetMeshTokenizeName(const string &name)
{
	vector<string> nameTokens;
	common::tokenizer(name, "::", "", nameTokens);
	string meshName;
	if (nameTokens.size() < 2)
	{
		meshName = name;
	}
	else
	{
		meshName = nameTokens[1];
	}

	return meshName.c_str();
}


void CModelPanel::OnSelchangedMaterialTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	const CString wItemText = m_MaterialTree.GetItemText(pNMTreeView->itemNew.hItem);
	const string itemText = wstr2str((wstring)wItemText);

	{
		const string searchToken = "diffuse texture = ";
		int pos = itemText.find(searchToken);
		if (pos != string::npos)
		{
			const string fileName = itemText.substr(pos + searchToken.length());
			g_modelView->ShowTexture(fileName);
		}
	}

	{
		const string searchToken = "specular texture = ";
		int pos = itemText.find(searchToken);
		if (pos != string::npos)
		{
			const string fileName = itemText.substr(pos + searchToken.length());
			g_modelView->ShowTexture(fileName);
		}
	}

	{
		const string searchToken = "bump texture = ";
		int pos = itemText.find(searchToken);
		if (pos != string::npos)
		{
			const string fileName = itemText.substr(pos + searchToken.length());
			g_modelView->ShowTexture(fileName);
		}
	}

	{
		const string searchToken = "selfIllum texture = ";
		int pos = itemText.find(searchToken);
		if (pos != string::npos)
		{
			const string fileName = itemText.substr(pos + searchToken.length());
			g_modelView->ShowTexture(fileName);
		}
	}

}
