//
// Zealot Volume Shadow
//

#include "stdafx.h"

using namespace graphic;


class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float elapseT) override;
	virtual void OnRender(const float elapseT) override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;


protected:
	bool GenerateShadowMesh();
	void RenderShadow();


private:
	cCharacter m_character;
	cCube3 m_cube2;
	cCube3 m_cube3;
	cShader m_shader;
	cShadowVolume m_shadowMesh;
	cShader m_zealotAmbientShader;
	cShader m_zealotSceneShader;
	cCamera m_lightCamera;
	bool m_isShowShadow;
	bool m_isPause;
	bool m_isRenderAmbient;
	bool m_isRenderShadow;
	bool m_isRenderScene;

	Matrix44 m_rotateTm;
	Matrix44 m_rotateTm2;
	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);


cViewer::cViewer()
	: m_isShowShadow(false)
	 , m_isPause(false)
	, m_isRenderAmbient(true)
	, m_isRenderShadow(true)
	, m_isRenderScene(true)
{
	m_windowName = L"Shadow Volume 2";
	const RECT r = { 0, 0, 1024, 768 };
	m_windowRect = r;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = 1024;		//초기 윈도우 가로 크기
	const int WINSIZE_Y = 768;	//초기 윈도우 세로 크기
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.f, 10000.0f);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.1f, 0.1f, 0.1f, 1));
	GetMainLight().SetPosition(Vector3(-300, 300, -300));
	GetMainLight().SetDirection(Vector3(1, -1, 1).Normal());
	m_lightCamera.Init(&m_renderer);
	m_lightCamera.SetCamera(Vector3(30, 30, 30), Vector3(0, 0, 0), Vector3(0, 1, 0));

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	// Load Zealot
	{
		vector<sActionData> actions;
		actions.reserve(16);
		actions.push_back(sActionData(CHARACTER_ACTION::RUN, "zealot_walk.ani"));
		actions.push_back(sActionData(CHARACTER_ACTION::ATTACK, "zealot_attack.ani"));

		m_character.Create(m_renderer, "zealot.dat");
		if (graphic::cMesh* mesh = m_character.GetMesh("Sphere001"))
			mesh->SetRender(false);
		m_character.SetShader(graphic::cResourceManager::Get()->LoadShader(
			m_renderer, "hlsl_skinning_using_texcoord_sc2.fx"));
		m_character.SetRenderShadow(true);
		m_character.SetActionData(actions);
		//m_character.Action(CHARACTER_ACTION::RUN);

		m_zealotAmbientShader.Create(m_renderer, "../media/shader/hlsl_skinning_using_texcoord_volumeshadow_ambient.fx", "TShader");
		m_zealotSceneShader.Create(m_renderer, "../media/shader/hlsl_skinning_using_texcoord_volumeshadow_scene.fx", "TShader");
	}

	if (!m_shader.Create(m_renderer, "../media/shader/hlsl_shadow.fx", "Shadow"))
		return false;

	m_cube2.InitCube(m_renderer);
	m_cube2.m_tm.SetTranslate(Vector3(3.5f, -2, 2));
	m_cube2.m_mtrl.InitBlue();

	m_cube3.SetCube(m_renderer, Vector3(-100, -5, -100), Vector3(100, -4, 100));

	GenerateShadowMesh();

	return true;
}


// Direct3D9 Shadow Volume Sample
bool cViewer::GenerateShadowMesh()
{
	HRESULT hr = S_OK;
	ID3DXMesh *pInputMesh = NULL;

	//--------------------------------------------------------------------------------------------//
	// Copy Cube to Mesh
	cShadowVolume::sVertex *pvtx = NULL;
	DWORD *pidx = NULL;

	for each (cMesh *mesh in m_character.m_meshes)
	{
		cMeshBuffer *meshBuff = mesh->GetMeshBuffer();
		if (!meshBuff)
			continue;

		hr = D3DXCreateMesh(meshBuff->m_idxBuff.GetFaceCount(),
			meshBuff->m_vtxBuff.GetVertexCount(), D3DXMESH_32BIT, cShadowVolume::sVertex::Decl, m_renderer.GetDevice(), &pInputMesh);
		if (FAILED(hr))
			return false;

		pInputMesh->LockVertexBuffer(0, (LPVOID*)&pvtx);
		pInputMesh->LockIndexBuffer(0, (LPVOID*)&pidx);
		if (!pvtx || !pidx)
			return false;

		BYTE *psrcVtx = (BYTE*)meshBuff->m_vtxBuff.Lock();
		WORD *psrcIdx = (WORD*)meshBuff->m_idxBuff.Lock();

		const int pos_offset = meshBuff->m_vtxBuff.GetOffset(D3DDECLUSAGE_POSITION);
		const int normal_offset = meshBuff->m_vtxBuff.GetOffset(D3DDECLUSAGE_NORMAL);
		const int stride = meshBuff->m_vtxBuff.GetSizeOfVertex();

		for (int i = 0; i < meshBuff->m_vtxBuff.GetVertexCount(); ++i)
		{
			pvtx->Position = *(Vector3*)(psrcVtx + pos_offset);
			pvtx->Normal = *(Vector3*)(psrcVtx + normal_offset);
			psrcVtx += stride;
			++pvtx;
		}

		for (int i = 0; i < meshBuff->m_idxBuff.GetFaceCount() * 3; ++i)
			*pidx++ = *psrcIdx++;

		meshBuff->m_vtxBuff.Unlock();
		meshBuff->m_idxBuff.Unlock();

		pInputMesh->UnlockIndexBuffer();
		pInputMesh->UnlockVertexBuffer();
		break;
	}
	//--------------------------------------------------------------------------------------------//

	m_shadowMesh.Create(m_renderer, pInputMesh);
	pInputMesh->Release();

	return true;
}


void cViewer::OnUpdate(const float elapseT)
{
	if (!m_isPause)
		m_character.Update(elapseT);

	// keyboard
	if (GetFocus() == m_hWnd)
	{
		const float vel = 10 * elapseT;
		if (GetAsyncKeyState('W'))
			GetMainCamera()->MoveFront(vel);
		else if (GetAsyncKeyState('A'))
			GetMainCamera()->MoveRight(-vel);
		else if (GetAsyncKeyState('D'))
			GetMainCamera()->MoveRight(vel);
		else if (GetAsyncKeyState('S'))
			GetMainCamera()->MoveFront(-vel);
		else if (GetAsyncKeyState('E'))
			GetMainCamera()->MoveUp(vel);
		else if (GetAsyncKeyState('C'))
			GetMainCamera()->MoveUp(-vel);

		GetMainCamera()->Update();
	}
}


void cViewer::RenderShadow()
{
	 // Ambient
	if (m_isRenderAmbient)
	{
		m_shader.SetTechnique("Ambient");

		// box2
		//m_shader.SetMatrix("mWorld", m_cube2.m_tm);
		//m_shader.SetMatrix("mVP", GetMainCamera()->GetViewProjectionMatrix());
		//m_shader.SetVector("g_vAmbient", Vector4(0.2f, 0.2f, 0.2f, 1.f));
		//m_shader.SetVector("g_vMatColor", Vector4(0, 0, 1, 1));

		//int passCount = m_shader.Begin();
		//for (int i = 0; i < passCount; ++i)
		//{
		//	m_shader.BeginPass(i);
		//	m_shader.CommitChanges();
		//	m_cube2.Render(m_renderer);
		//	m_shader.EndPass();
		//}
		//m_shader.End();


		m_character.SetShader(&m_zealotAmbientShader);
		m_character.Render(m_renderer, m_rotateTm);


		// box3
		//m_shader.SetMatrix("mWorld", m_cube3.m_tm);
		//m_shader.SetMatrix("mVP", GetMainCamera()->GetViewProjectionMatrix());
		//m_shader.SetVector("g_vAmbient", Vector4(0.2f, 0.2f, 0.2f, 1.f));
		//m_shader.SetVector("g_vMatColor", Vector4(1, 1, 1, 1));

		//passCount = m_shader.Begin();
		//for (int i = 0; i < passCount; ++i)
		//{
		//	m_shader.BeginPass(i);
		//	m_shader.CommitChanges();
		//	m_cube3.Render(m_renderer);
		//	m_shader.EndPass();
		//}
		//m_shader.End();
	}


	m_renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 170, 170, 170), 1.0f, 0);

	// Shadow
	if (m_isRenderShadow)
	{
		if (m_isShowShadow)
			m_shader.SetTechnique("ShowShadow");
		else
			m_shader.SetTechnique("Shadow");

		Matrix44 mWorldView = m_rotateTm *  GetMainCamera()->GetViewMatrix();
		m_shader.SetMatrix("g_mWorld", m_rotateTm);
		m_shader.SetMatrix("g_mView", GetMainCamera()->GetViewMatrix());		
		m_shader.SetMatrix("g_mVP", GetMainCamera()->GetViewProjectionMatrix());
		m_shader.SetMatrix("g_mProj", GetMainCamera()->GetProjectionMatrix());
		m_shader.SetVector("g_vLightView", GetMainLight().GetPosition() * GetMainCamera()->GetViewMatrix());
		m_shader.SetVector("g_vShadowColor", Vector4(0, 1, 0, 0.2f));
		m_shader.SetFloat("g_fFarClip", 10000.0f);

		const int passCount = m_shader.Begin();
		for (int i = 0; i < passCount; ++i)
		{
			m_shader.BeginPass(i);
			m_shader.CommitChanges();
			m_shadowMesh.Render(m_renderer);
			m_shader.EndPass();
		}
		m_shader.End();
	}


	// Scene
	if (m_isRenderScene)
	{
		// box2
		m_shader.SetTechnique("Scene");

		m_shader.SetMatrix("g_mWorld", m_cube2.m_tm);
		Matrix44 mWorldView = m_cube2.m_tm *  GetMainCamera()->GetViewMatrix();
		m_shader.SetMatrix("g_mVP", GetMainCamera()->GetViewProjectionMatrix());
		Matrix44 wit = m_cube2.m_tm.Inverse();
		wit.Transpose();
		m_shader.SetMatrix("g_mWIT", wit);
		m_shader.SetVector("K_d", Vector4(0, 0, 0.7f, 0));

		int passCount = m_shader.Begin();
		for (int i = 0; i < passCount; ++i)
		{
			m_shader.BeginPass(i);
			m_shader.CommitChanges();
			m_cube2.Render(m_renderer);
			m_shader.EndPass();
		}
		m_shader.End();

		// box3
		{
			m_shader.SetMatrix("g_mWorld", m_cube3.m_tm);
			Matrix44 mWorldView = m_cube3.m_tm *  GetMainCamera()->GetViewMatrix();
			m_shader.SetMatrix("g_mWorldView", mWorldView);
			m_shader.SetMatrix("g_mVP", GetMainCamera()->GetViewProjectionMatrix());
			m_shader.SetVector("K_d", Vector4(0.7f, .7f, .7f, 0));

			int passCount = m_shader.Begin();
			for (int i = 0; i < passCount; ++i)
			{
				m_shader.BeginPass(i);
				m_shader.CommitChanges();
				m_cube3.Render(m_renderer, m_rotateTm);
				m_shader.EndPass();
			}
			m_shader.End();
		}

		m_character.SetShader(&m_zealotSceneShader);
		m_character.Render(m_renderer, m_rotateTm);

	}
}


void cViewer::OnRender(const float elapseT)
{
	GetMainLight().Bind(m_renderer, 0);

	//if (m_renderer.ClearScene())
	m_renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 66, 75, 121), 1.0f, 0);
	{
		m_renderer.BeginScene();

		m_renderer.RenderGrid();
		m_renderer.RenderAxis();
		m_renderer.RenderFPS();

		RenderShadow();

		m_renderer.EndScene();
		m_renderer.Present();
	}
}


void cViewer::OnShutdown()
{
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEWHEEL:
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera()->GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera()->Zoom((zDelta<0) ? -zoomLen : zoomLen);
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
		{
			static bool flag = false;
			m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;
		case VK_SPACE:
			m_isShowShadow = !m_isShowShadow;
			break;
		case 'P':
			m_isPause = !m_isPause;
			break;
		case VK_RETURN:
		{
			m_shader.Create(m_renderer, "../media/shader/hlsl_shadow.fx", "Shadow");
			m_zealotAmbientShader.Create(m_renderer, "../media/shader/hlsl_skinning_using_texcoord_volumeshadow_ambient.fx", "TShader");
			m_zealotSceneShader.Create(m_renderer, "../media/shader/hlsl_skinning_using_texcoord_volumeshadow_scene.fx", "TShader");
		}
		break;
		case '1': m_isRenderAmbient = !m_isRenderAmbient; break;
		case '2': m_isRenderShadow = !m_isRenderShadow; break;
		case '3': m_isRenderScene = !m_isRenderScene; break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		m_LButtonDown = false;
		break;

	case WM_RBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_RButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);

		// unit 이동.
		Ray ray(m_curPos.x, m_curPos.y, 1024, 768,
			GetMainCamera()->GetProjectionMatrix(),
			GetMainCamera()->GetViewMatrix());
	}
	break;

	case WM_RBUTTONUP:
		m_RButtonDown = false;
		ReleaseCapture();
		break;

	case WM_MBUTTONDOWN:
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (wParam & 0x10) // middle button down
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
		}

		if (m_LButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			Quaternion q1(graphic::GetMainCamera()->GetRight(), -y * 0.01f);
			Quaternion q2(graphic::GetMainCamera()->GetUpVector(), -x * 0.01f);

			if (GetAsyncKeyState('F'))
			{
				//m_cube2.m_tm *= (q2.GetMatrix() * q1.GetMatrix());
			}
			else
			{
				m_rotateTm *= (q2.GetMatrix() * q1.GetMatrix());
			}

		}
		else if (m_RButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			//if (GetAsyncKeyState('C'))
			{
				graphic::GetMainCamera()->Yaw2(x * 0.005f);
				graphic::GetMainCamera()->Pitch2(y * 0.005f);
			}
		}
		else if (m_MButtonDown)
		{
			const POINT point = { LOWORD(lParam), HIWORD(lParam) };
			const POINT pos = { point.x - m_curPos.x, point.y - m_curPos.y };
			m_curPos = point;

			const float len = graphic::GetMainCamera()->GetDistance();
			graphic::GetMainCamera()->MoveRight(-pos.x * len * 0.001f);
			graphic::GetMainCamera()->MoveUp(pos.y * len * 0.001f);
		}
		else
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
		}

	}
	break;
	}
}

