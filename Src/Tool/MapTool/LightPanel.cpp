// LightPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "LightPanel.h"
#include "afxdialogex.h"


using namespace graphic;

enum {
	ID_PROPERTY_MATERIAL = 100,
	ID_PROPERTY_LIGHT,
};


CLightPanel::CLightPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(CLightPanel::IDD, pParent)
	, m_EditDirection(FALSE)
{

}

CLightPanel::~CLightPanel()
{
}

void CLightPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_LIGHT_DIRECTION, m_EditDirection);
}


BEGIN_MESSAGE_MAP(CLightPanel, CPanelBase)
	ON_BN_CLICKED(IDOK, &CLightPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLightPanel::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE (AFX_WM_PROPERTY_CHANGED, OnPropertyChanged) 
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_LIGHT_DIRECTION, &CLightPanel::OnBnClickedCheckLightDirection)
END_MESSAGE_MAP()


// CLightPanel 메시지 처리기입니다.


void CLightPanel::OnBnClickedOk()
{
	//CPanelBase::OnOK();
}


void CLightPanel::OnBnClickedCancel()
{
	//CPanelBase::OnCancel();
}


BOOL CLightPanel::OnInitDialog()
{
	CPanelBase::OnInitDialog();


	//----------------------------------------------------------------------------------------------
	// Init PropertyGrid Control
	if (!m_modelProperty.Create(WS_VISIBLE | WS_CHILD, CRect(0,390,400,650), this, ID_PROPERTY_MATERIAL))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	m_modelProperty.EnableHeaderCtrl(FALSE);
	m_modelProperty.SetFocus();
	m_modelProperty.SetVSDotNetLook();
	m_modelProperty.MarkModifiedProperties();


	if (!m_lightProperty.Create(WS_VISIBLE | WS_CHILD, CRect(0,40,400,350), this, ID_PROPERTY_LIGHT))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	m_lightProperty.EnableHeaderCtrl(FALSE);
	m_lightProperty.SetFocus();
	m_lightProperty.SetVSDotNetLook();
	m_lightProperty.MarkModifiedProperties();


	//UpdateModelProperty(cController::Get()->GetCube().GetMaterial());
	UpdateLightProperty(cLightManager::Get()->GetMainLight());

	return TRUE;
}



void CLightPanel::UpdateModelProperty(const cMaterial &mtrl)
{
	m_modelProperty.RemoveAll();

	CMFCPropertyGridProperty* group = new CMFCPropertyGridProperty(_T("Cube Property"));

	CString types[] = {
		_T("Cube"),
		_T("Sphere"),
	};
	CMFCPropertyGridProperty* typeProp = new CMFCPropertyGridProperty(_T("Model"),
		types[ 0] );
		//types[ cController::Get()->GetSelectModel()] );
	typeProp->AddOption(types[0]);
	typeProp->AddOption(types[1]);
	typeProp->AllowEdit(FALSE);
	group->AddSubItem(typeProp);

	AddPropertyColor4(group, L"Diffuse", *(Vector4*)&mtrl.m_mtrl.Diffuse);
	AddPropertyColor4(group, L"Ambient", *(Vector4*)&mtrl.m_mtrl.Ambient);
	AddPropertyColor4(group, L"Specular", *(Vector4*)&mtrl.m_mtrl.Specular);
	AddPropertyColor4(group, L"Emissive", *(Vector4*)&mtrl.m_mtrl.Emissive);
	group->AddSubItem(new CPropGridSlider(_T("Power"), mtrl.m_mtrl.Power, L"", 0, 128, 100));

	COleVariant varSpecular((short)VARIANT_TRUE, VT_BOOL);
	group->AddSubItem(new CMFCPropertyGridProperty(_T("Specular Effect"), varSpecular, L""));

	m_modelProperty.AddProperty( group);
}


void CLightPanel::UpdateLightProperty(const cLight &light)
{
	m_lightProperty.RemoveAll();

	CMFCPropertyGridProperty* group = new CMFCPropertyGridProperty(_T("Light Property"));

	CString types[] = {
		_T("None"),
		_T("Point Light"),
		_T("SpotLight"),
		_T("Directional Light"),
	};
	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Type"),
		types[ light.m_light.Type] );
	pProp->AddOption(types[0]);
	pProp->AddOption(types[1]);
	pProp->AddOption(types[2]);
	pProp->AddOption(types[3]);
	pProp->AllowEdit(FALSE);
	group->AddSubItem(pProp);

	AddPropertyColor4(group, L"Diffuse", *(Vector4*)&light.m_light.Diffuse);
	AddPropertyColor4(group, L"Specular", *(Vector4*)&light.m_light.Specular);
	AddPropertyColor4(group, L"Ambient", *(Vector4*)&light.m_light.Ambient);
	AddPropertyVector3(group, L"Position", *(Vector3*)&light.m_light.Position,
		-500, 500, 500);
	AddPropertyVector3(group, L"Direction", *(Vector3*)&light.m_light.Direction,
		-1, 1, 100);

	group->AddSubItem(new CPropGridSlider(_T("Range"), light.m_light.Range, L"", 0, 500, 1000) );
	group->AddSubItem(new CPropGridSlider(_T("Falloff"), light.m_light.Falloff, L"", 0, 2, 1000));
	group->AddSubItem(new CPropGridSlider(_T("Attenuation0"), light.m_light.Attenuation0, L"", 0, 1.f, 1000));
	group->AddSubItem(new CPropGridSlider(_T("Attenuation1"), light.m_light.Attenuation1, L"", 0, 0.1f, 1000));
	group->AddSubItem(new CPropGridSlider(_T("Attenuation2"), light.m_light.Attenuation2, L"", 0, 0.1f, 1000));
	group->AddSubItem(new CPropGridSlider(_T("Theta"), light.m_light.Theta, L"", 0, 3.14f, 100));
	group->AddSubItem(new CPropGridSlider(_T("Phi"), light.m_light.Phi, L"", 0, 3.14f, 100) );

	group->AdjustButtonRect();
	group->AllowEdit();
	group->Enable();
	group->Show();
	group->Redraw();

	m_lightProperty.AddProperty( group);
}


void CLightPanel::AddPropertyColor4(CMFCPropertyGridProperty *group, CString name, 
	Vector4 value)
{
	CMFCPropertyGridProperty *pProp;

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(name, 0, TRUE);
	pProp = new CPropGridSlider(_T("r"), (_variant_t)value.x, L"", 0, 1, 100);
	pSize->AddSubItem(pProp);

	pProp = new CPropGridSlider( _T("g"), (_variant_t)value.y, L"", 0, 1, 100);
	pSize->AddSubItem(pProp);

	pProp = new CPropGridSlider( _T("b"), (_variant_t)value.z, L"", 0, 1, 100);
	pSize->AddSubItem(pProp);

	pProp = new CPropGridSlider( _T("a"), (_variant_t)value.w, L"", 0, 1, 100);
	pSize->AddSubItem(pProp);

	group->AddSubItem(pSize);
}


void CLightPanel::AddPropertyVector3(CMFCPropertyGridProperty *group, CString name, 
	Vector3 value, const float _min, const float _max, const int slice)
{
	CMFCPropertyGridProperty *pProp;

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(name, 0, TRUE);
	pProp = new CPropGridSlider(_T("x"), (_variant_t)value.x, L"", _min, _max, slice);
	pSize->AddSubItem(pProp);

	pProp = new CPropGridSlider( _T("y"), (_variant_t)value.y, L"", _min, _max, slice);
	pSize->AddSubItem(pProp);

	pProp = new CPropGridSlider( _T("z"), (_variant_t)value.z, L"", _min, _max, slice);
	pSize->AddSubItem(pProp);

	group->AddSubItem(pSize);
}


LRESULT CLightPanel::OnPropertyChanged (WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty *prop = (CMFCPropertyGridProperty*)lparam;

	switch (wparam)
	{
	case ID_PROPERTY_MATERIAL:
		{
			if ((CString(L"r") == prop->GetName()) || 
				(CString(L"g") == prop->GetName()) ||
				(CString(L"b") == prop->GetName()) || 
				(CString(L"a") == prop->GetName()) )
			{
				CMFCPropertyGridProperty *parent = prop->GetParent();
				ChangeMaterialValue(parent);
			}
			else
			{
				ChangeMaterialValue(prop);
			}
		}
		break;

	case ID_PROPERTY_LIGHT:
		{
			if ((CString(L"r") == prop->GetName()) || 
				(CString(L"g") == prop->GetName()) ||
				(CString(L"b") == prop->GetName()) || 
				(CString(L"a") == prop->GetName()) ||
				(CString(L"x") == prop->GetName()) ||
				(CString(L"y") == prop->GetName()) ||
				(CString(L"z") == prop->GetName()) ||
				(CString(L"w") == prop->GetName()) )
			{
				CMFCPropertyGridProperty *parent = prop->GetParent();
				ChangeLightValue(parent);
			}
			else
			{
				ChangeLightValue(prop);
			}
		}
		break;
	}

	return 0;
}


void CLightPanel::ChangeMaterialValue(CMFCPropertyGridProperty *prop)
{
	RET(!prop);
/*
	cCube2 &cube = cController::Get()->GetCube();
	cSphere &sphere = cController::Get()->GetSphere();
	cMaterial &material = (cController::Get()->GetSelectModel()==0)? 
		cube.GetMaterial() : sphere.GetMaterial();

	if (CString(L"Model") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		CString type = value;
		for (int i=0; i < prop->GetOptionCount(); ++i)
		{
			if (prop->GetOption(i) == type)
			{
				cController::Get()->SetSelectModel(i);
				switch (i)
				{
				case 0: UpdateModelProperty(cube.GetMaterial()); break;
				case 1: UpdateModelProperty(sphere.GetMaterial()); break;
				}
				break;
			}
		}
	}
	else if (CString(L"Power") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		material.m_mtrl.Power = value;
	}
	else if (CString(L"Diffuse") == prop->GetName())
	{
		const Vector4 value= GetPropertyVector4(prop);
		material.m_mtrl.Diffuse = *(D3DXCOLOR*)&value;
	}
	else if (CString(L"Ambient") == prop->GetName())
	{
		const Vector4 value = GetPropertyVector4(prop);
		material.m_mtrl.Ambient = *(D3DXCOLOR*)&value;
	}
	else if (CString(L"Specular") == prop->GetName())
	{
		const Vector4 value = GetPropertyVector4(prop);
		material.m_mtrl.Specular = *(D3DXCOLOR*)&value;
	}
	else if (CString(L"Emissive") == prop->GetName())
	{
		const Vector4 value = GetPropertyVector4(prop);
		material.m_mtrl.Emissive = *(D3DXCOLOR*)&value;
	}
	else if (CString(L"Specular Effect") == prop->GetName())
	{
		_variant_t value = prop->GetValue();
		cController::Get()->EnableSpecularEffect(value);
	}
/**/
}


void CLightPanel::ChangeLightValue(CMFCPropertyGridProperty *prop)
{
	RET(!prop);

	cLight &light = cLightManager::Get()->GetMainLight();

	if (CString(L"Diffuse") == prop->GetName())
	{
		const Vector4 value= GetPropertyVector4(prop);
		light.m_light.Diffuse = *(D3DXCOLOR*)&value;
	}
	else if (CString(L"Ambient") == prop->GetName())
	{
		const Vector4 value = GetPropertyVector4(prop);
		light.m_light.Ambient = *(D3DXCOLOR*)&value;
	}
	else if (CString(L"Specular") == prop->GetName())
	{
		const Vector4 value = GetPropertyVector4(prop);
		light.m_light.Specular = *(D3DXCOLOR*)&value;
	}
	else if (CString(L"Position") == prop->GetName())
	{
		const Vector3 value = GetPropertyVector3(prop);
		light.m_light.Position = *(D3DXVECTOR3*)&value;
	}
	else if (CString(L"Direction") == prop->GetName())
	{
		const Vector3 value = GetPropertyVector3(prop);
		light.m_light.Direction = *(D3DXVECTOR3*)&value;
	}	
	else if (CString(L"Range") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		light.m_light.Range = value;
	}
	else if (CString(L"Falloff") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		light.m_light.Falloff = value;
	}
	else if (CString(L"Attenuation0") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		light.m_light.Attenuation0 = value;
	}
	else if (CString(L"Attenuation1") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		light.m_light.Attenuation1 = value;
	}
	else if (CString(L"Attenuation2") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		light.m_light.Attenuation2 = value;
	}
	else if (CString(L"Theta") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		light.m_light.Theta = value;
	}
	else if (CString(L"Phi") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		light.m_light.Phi = value;
	}
	else if (CString(L"Type") == prop->GetName())
	{
		const _variant_t value = prop->GetValue();
		CString type = value;
		for (int i=0; i < prop->GetOptionCount(); ++i)
		{
			if (prop->GetOption(i) == type)
			{
				light.m_light.Type = (D3DLIGHTTYPE)i;
				break;
			}
		}
	}
/**/
}


Vector4 CLightPanel::GetPropertyVector4(CMFCPropertyGridProperty *group)
{
	CMFCPropertyGridProperty *propX = group->GetSubItem(0);
	CMFCPropertyGridProperty *propY = group->GetSubItem(1);
	CMFCPropertyGridProperty *propZ = group->GetSubItem(2);
	CMFCPropertyGridProperty *propW = group->GetSubItem(3);

	_variant_t x = propX->GetValue();
	_variant_t y = propY->GetValue();
	_variant_t z = propZ->GetValue();
	_variant_t w = propW->GetValue();

	return Vector4(x, y, z, w);
}


Vector3 CLightPanel::GetPropertyVector3(CMFCPropertyGridProperty *group)
{
	CMFCPropertyGridProperty *propX = group->GetSubItem(0);
	CMFCPropertyGridProperty *propY = group->GetSubItem(1);
	CMFCPropertyGridProperty *propZ = group->GetSubItem(2);

	_variant_t x = propX->GetValue();
	_variant_t y = propY->GetValue();
	_variant_t z = propZ->GetValue();

	return Vector3(x, y, z);
}


void CLightPanel::OnSize(UINT nType, int cx, int cy)
{
	CPanelBase::OnSize(nType, cx, cy);

	MoveChildCtrlWindow(m_modelProperty, cx, cy);
	MoveChildCtrlWindow(m_lightProperty, cx, cy);	
}


void CLightPanel::Update(int type)
{
	switch (type)
	{
	case NOTIFY_TYPE::NOTIFY_CHANGE_TERRAIN:
		UpdateLightProperty(cLightManager::Get()->GetMainLight());
		break;

	case NOTIFY_TYPE::NOTIFY_UPDATE_LIGHT_DIRECTION: // 조명 방향 수정
		{
			// root property
			if (CMFCPropertyGridProperty *group = m_lightProperty.GetProperty(0)) 
			{
				// direction property
				if (CMFCPropertyGridProperty *prop = group->GetSubItem(5)) 
				{
					cLight &light = cLightManager::Get()->GetMainLight();
					prop->GetSubItem(0)->SetValue(light.m_light.Direction.x);
					prop->GetSubItem(1)->SetValue(light.m_light.Direction.y);
					prop->GetSubItem(2)->SetValue(light.m_light.Direction.z);
				}
			}
		}
		break;
	}

}


void CLightPanel::OnBnClickedCheckLightDirection()
{
	UpdateData();	
	if (m_EditDirection)
		::AfxMessageBox(L"화면에 마우스 왼쪽 버튼을 눌러서 조명의 방향을 조정할 수 있습니다.");
}
