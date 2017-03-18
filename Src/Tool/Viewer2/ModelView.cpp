// ModelView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "ModelView.h"
#include "LightPanel.h"
#include "Viewer2Dlg.h"


using namespace graphic;


// CModelView
CModelView::CModelView()
	: m_sprite(NULL)
	, m_spr(NULL)
{
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;	
	m_showSkybox = false;
}

CModelView::~CModelView()
{
	SAFE_DELETE(m_spr);
	SAFE_DELETE(m_sprite);
	//SAFE_RELEASE(m_dxSprite);
}

BEGIN_MESSAGE_MAP(CModelView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CModelView �����Դϴ�.

#ifdef _DEBUG
void CModelView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CModelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CModelView �޽��� ó�����Դϴ�.

void CModelView::Init()
{
	const int WINSIZE_X = 1024;		//�ʱ� ������ ���� ũ��
	const int WINSIZE_Y = 768;	//�ʱ� ������ ���� ũ��
	GetMainCamera()->Init(g_renderer);
	GetMainCamera()->SetCamera(Vector3(100,300,-500), Vector3(0,0,0), Vector3(0,1,0));
	GetMainCamera()->SetProjection( D3DX_PI / 4.f, (float)WINSIZE_X / (float) WINSIZE_Y, 1.f, 10000.0f) ;

	g_renderer->GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	g_renderer->GetDevice()->LightEnable (0,true);
	cLightManager::Get()->GetMainLight().Bind(*g_renderer, 0);

	//m_shader.Create( "./media/shader/hlsl_skinning_using_color.fx", "TShader" );
	//m_shader.Create( "./media/shader/hlsl_skinning_using_texcoord.fx", "TShader" );
	//m_shader.Create(*g_renderer,  "../media/shader/hlsl_rigid_phong.fx", "TShader" );
	m_shader = cResourceManager::Get()->LoadShader(*g_renderer, "../media/shader/hlsl_rigid_phong.fx", "TShader");
	m_skybox.Create(*g_renderer, "../media/skybox" );
	
	m_grid.Create(*g_renderer, 64, 64, 50, 1);
	m_grid.GetTexture().Create(*g_renderer, "../media/texture/emptyTexture.png" );
	m_grid.GetMaterial().Init(Vector4(0.4f,0.4f,0.4f,1), Vector4(0.7f,0.7f,0.7f,1),
		Vector4(1,1,1,1));

	m_msg.Create(*g_renderer);
	m_msg.SetText(10, 740, "press 'F' to focus model" );

	m_lightLine.GetMaterial().InitBlack();
	m_lightLine.GetMaterial().GetMtrl().Emissive = *(D3DXCOLOR*)&Vector4(1,1,0,1);

	m_lightSphere.Create(*g_renderer, 5, 10, 10);
	m_lightSphere.m_mtrl.InitBlack();
	m_lightSphere.m_mtrl.GetMtrl().Emissive = *(D3DCOLORVALUE*)&Vector4(1,1,0,1);

	m_spr = new cSprite();
	m_sprite = new cSprite2(*m_spr, 0);

	cController::Get()->AddObserver(this);
}


void CModelView::Update(const float elapseT)
{
	g_renderer->Update(elapseT);
	cController::Get()->Update(elapseT);
}


void CModelView::Render()
{
	if (g_renderer->ClearScene())
	{
		if (cCharacter *character = cController::Get()->GetCharacter())
			character->UpdateShadow(*g_renderer);

		GetMainCamera()->Bind(*m_shader);
		GetMainLight().Bind(*m_shader);

		g_renderer->GetDevice()->BeginScene();

		g_renderer->GetDevice()->SetTransform(D3DTS_WORLD, ToDxM(Matrix44::Identity) );

		if (m_showSkybox)
			m_skybox.Render(*g_renderer);

		if (cCharacter *character = cController::Get()->GetCharacter())
		{
			character->SetRenderShadow(true);
			character->SetTransform(m_rotateTm);

			// �׸��� �ؽ���, ��Ʈ���� ����.
			Vector3 lightPos;
			Matrix44 view, proj, tt;
			cLightManager::Get()->GetMainLight().GetShadowMatrix(
				Vector3(0,0,0), lightPos, view, proj, tt );
			m_shader->SetRenderPass(3);

			m_shader->SetMatrix( "g_mWVPT", view * proj * tt );
			m_shader->SetTexture("g_ShadowMap", character->GetShadow().GetTexture());

			if (character->m_shader)
			{
				GetMainCamera()->Bind(*character->m_shader);
				GetMainLight().Bind(*character->m_shader);
			}
		}

		// �ٴ� �׸��� ���.
		// ���̴� ���� �ʱ�ȭ.
		//cLightManager::Get()->GetMainLight().Bind(*m_shader);
		m_shader->SetVector( "g_vFog", Vector3(1.f, 10000.f, 0)); // near, far

		// �ٴ� ���. 
		m_shader->SetTechnique("Scene");
		m_grid.RenderShader(*g_renderer, *m_shader);

		// ��׶��� �׸���, �� ���.
		g_renderer->RenderGrid();
		g_renderer->RenderAxis();
		g_renderer->RenderFPS();
		m_msg.Render();

		// ĳ���� ���.
		m_shader->SetTechnique("TShader");
		cController::Get()->Render();

		// ���� ���� �� ���.
		if (cController::Get()->GetCurrentPanel() == EDIT_MODE::LIGHT)
		{
			// ���� ��ġ ���� �� ���.
			const Vector3 lightPos = cLightManager::Get()->GetMainLight().GetPosition();
			Matrix44 lightTm;
			lightTm.SetTranslate( lightPos );
			m_lightSphere.SetTransform(lightTm);
			m_lightSphere.Render(*g_renderer, Matrix44::Identity);

			if (m_LButtonDown 
				&& g_lightPanel->m_EditDirection)
			{
				m_lightLine.Render(*g_renderer);
			}
		}

		if (m_sprite)
			m_sprite->Render(*g_renderer, Matrix44::Identity);

		g_renderer->GetDevice()->EndScene();
		g_renderer->GetDevice()->Present( NULL, NULL, NULL, NULL );
	}
}


void CModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	SetFocus();
	m_LButtonDown = true;
	m_curPos = point;
	CView::OnLButtonDown(nFlags, point);
}


void CModelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_LButtonDown = false;
	CView::OnLButtonUp(nFlags, point);
}


void CModelView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_LButtonDown)
	{
		const CPoint pos = point  - m_curPos;
		m_curPos = point;
		
		bool isEditLightDirection = false;
		if (cController::Get()->GetCurrentPanel() == EDIT_MODE::LIGHT)
		{
			isEditLightDirection = g_lightPanel->m_EditDirection? true : false;
		}

		if (isEditLightDirection)
		{
			int width, height;
			switch (g_viewerDlg->GetDisplayMode())
			{
			case DISP_MODE::DISP_800X600_RIGHT: width = 800; height = 600; break; // 800X600 right align
			case DISP_MODE::DISP_1024X768_RIGHT: width = 1024; height = 768; break; // 1024X768 right align
			case DISP_MODE::DISP_800X600_LEFT: width = 800; height = 600; break; // 800X600 left align
			case DISP_MODE::DISP_1024X768_LEFT: width = 1024; height = 768; break; // 1024X768 left align
			default: return;
			}

			Ray ray(point.x, point.y, width, height, GetMainCamera()->GetProjectionMatrix(), 
				GetMainCamera()->GetViewMatrix());
			Vector3 pickPos;
			if (m_grid.Pick( ray.orig, ray.dir, pickPos ))
			{
				// ���� ���� ���ϱ�.
				const Vector3 lightPos = cLightManager::Get()->GetMainLight().GetPosition();
				Vector3 dir = pickPos - lightPos;
				dir.Normalize();
				m_lightLine.SetLine(*g_renderer, lightPos, pickPos, 1);

				// ���� ��ġ ����
				Matrix44 lightTm;
				lightTm.SetTranslate( lightPos );
				m_lightSphere.SetTransform(lightTm);

				// ���� ���� ������Ʈ
				cLightManager::Get()->GetMainLight().SetDirection(dir);

				// ��� �����ʿ��� �����Ѵ�.
				cController::Get()->SendUpdate(NOTIFY_MSG::UPDATE_LIGHT_DIRECTION);
			}
		}
		else
		{
			Quaternion q1(GetMainCamera()->GetRight(), -pos.y * 0.01f);
			Quaternion q2(GetMainCamera()->GetUpVector(), -pos.x * 0.01f);
			m_rotateTm *= (q2.GetMatrix() * q1.GetMatrix());
		}

	}	
	else if (m_RButtonDown)
	{
		const CPoint pos = point  - m_curPos;
		m_curPos = point;

		GetMainCamera()->Yaw2( pos.x * 0.005f );
		GetMainCamera()->Pitch2( pos.y * 0.005f );
	}
	else if (m_MButtonDown)
	{
		const CPoint pos = point  - m_curPos;
		m_curPos = point;

		const float len = GetMainCamera()->GetDistance();
		GetMainCamera()->MoveRight( -pos.x * len * 0.001f );
		GetMainCamera()->MoveUp( pos.y * len * 0.001f );
	}

	CView::OnMouseMove(nFlags, point);
}


BOOL CModelView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	dbg::Print( "%d %d", nFlags, zDelta);

	const float len = GetMainCamera()->GetDistance();
	float zoomLen = (len > 100)? 50 : (len/4.f);
	if (nFlags & 0x4)
		zoomLen = zoomLen/10.f;

	GetMainCamera()->Zoom( (zDelta<0)? -zoomLen : zoomLen );

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CModelView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_RButtonDown = true;
	m_curPos = point;
	CView::OnRButtonDown(nFlags, point);
}


void CModelView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_RButtonDown = false;
	CView::OnRButtonUp(nFlags, point);
}


void CModelView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	SetCapture();
	m_MButtonDown = true;
	m_curPos = point;
	CView::OnMButtonDown(nFlags, point);
}


void CModelView::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_MButtonDown = false;
	CView::OnMButtonUp(nFlags, point);
}


void CModelView::Update(int type)
{
	if (NOTIFY_MSG::CHANGE_PANEL == type)
	{
		if (cController::Get()->GetCurrentPanel() == EDIT_MODE::LIGHT)
		{
			// ���� ��ġ ����
			const Vector3 lightPos = cLightManager::Get()->GetMainLight().GetPosition();
			Matrix44 lightTm;
			lightTm.SetTranslate( lightPos );
			m_lightSphere.SetTransform(lightTm);
		}
	}
	else if (NOTIFY_MSG::UPDATE_TEXTURE == type)
	{

	}
}


void CModelView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'F')
	{ // focus model
		// ���� ȭ�鿡 �� ���̰� �ڵ����� �����Ѵ�.
		if (cCharacter *character = cController::Get()->GetCharacter())
		{
			if (cBoundingBox *box = character->GetCollisionBox(*g_renderer))
			{
				const float len = box->Length();
				if (len <= 0)
					return;
				
				Vector3 eyePos = GetMainCamera()->GetEyePos().Normal() * len*2;
				GetMainCamera()->SetLookAt(box->Center());
				GetMainCamera()->SetEyePos(eyePos);
			}
		}		
	}
	else if (nChar == VK_ESCAPE)
	{
		if (m_sprite)
			m_sprite->Clear();
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CModelView::ShowTexture(const string &fileName)
{
	if (m_sprite)
	{
		m_sprite->SetTexture(*g_renderer, fileName);

		// 300 X 300 ������� ��ĳ�ϸ��Ѵ�.
		const float w = (float)m_sprite->m_rect.Width();
		const float h = (float)m_sprite->m_rect.Height();
		m_sprite->m_scale = Vector3(300/w, 300/h, 1);

		int width, height;
		GetViewSize(g_viewerDlg->GetDisplayMode(), width, height);
		m_sprite->m_pos = Vector3((float)width-300.f, 0, 0);
	}
}
