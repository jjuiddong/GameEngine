//
// Collada Viewer
//

#include "stdafx.h"
using namespace graphic;


class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float deltaSeconds) override;
	virtual void OnRender(const float deltaSeconds) override;
	virtual void OnDeviceLost() override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void ChangeWindowSize();


private:
	cImGui m_gui;
	cGrid3 m_ground;
	cSkyBox m_skybox;
	cShadowMap m_shadowMap;
	vector<cModel2*> m_models;
	vector<cShader*> m_shaders;

	bool m_isShadow = true;
	cCube m_cube1;
	cCube3 m_cube2;
	Matrix44 m_rotateTm;
	Matrix44 m_rotateTm2;
	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);


cViewer::cViewer()
{
	m_windowName = L"Rendering Test";
	const RECT r = { 0, 0, 1024, 768 };
	//const RECT r = { 0, 0, 1280, 1024};
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
	DragAcceptFiles(m_hWnd, TRUE);

	cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = 1024;
	const int WINSIZE_Y = 768;
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(15, 2, -5), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 100.0f);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-10, 10, -10);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	m_cube1.InitCube(m_renderer);
	m_cube1.SetColor(D3DXCOLOR(1, 0, 0, 1));

	m_cube2.InitCube(m_renderer);
	m_cube2.m_tm.SetTranslate(Vector3(3.5f, -2, 2));
	m_cube2.m_mtrl.InitBlue();

	m_gui.Init(m_hWnd, m_renderer.GetDevice());
	m_gui.SetContext();

	m_ground.Create(m_renderer, 10, 10, 100, 200);
	m_ground.m_tex = cResourceManager::Get()->LoadTexture(m_renderer, "../media/terrain/¹Ù´Ú.jpg");

	m_skybox.Create(m_renderer, "skybox");
	//m_shadowMap.Create(m_renderer, 1024, 1024);
	m_shadowMap.Create(m_renderer, 2048, 2048);

	{
		string files[] = {
			"ChessBishop.x"
			, "ChessKing.x"
			, "ChessKnight.x"
			, "ChessPawn.x"
			, "ChessQueen.x"
			, "ChessRook.x"
		};
		const int size = sizeof(files) / sizeof(string);
		for (int k = 0; k < 10; ++k)
		{
			for (int i = 0; i < size; ++i)
			{
				cModel2 *model = new cModel2();
				model->Create(m_renderer, files[i], "", "", true, false);

				Matrix44 T;
				T.SetPosition(Vector3(k*2.f, 0, i * 2.f) - Vector3(10,0,0));
				Matrix44 S;
				S.SetScale(Vector3(1, 1, 1) * 30);
				model->m_tm = S * T;			

				m_models.push_back(model);
			}		
		}
	}

	{
		string files[] = {
			//"collada_skin.fx"
			// "collada_rigid.fx"
			"xfile.fx"
			, "cube3.fx"
		};
		const int size = sizeof(files) / sizeof(string);
		for (int i = 0; i < size; ++i)
		{
			cShader *p = cResourceManager::Get()->LoadShader(m_renderer, files[i]);
			if (p)
			{
				m_shadowMap.Bind(*p, "g_shadowMapTexture");
				m_shaders.push_back(p);
			}
		}

		m_ground.SetShader(cResourceManager::Get()->LoadShader(m_renderer, files[1]));
	
	}

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	for (auto &p : m_models)
		p->Update(m_renderer, deltaSeconds);

	// keyboard
	if (GetFocus() == m_hWnd)
	{
		const float vel = 10 * deltaSeconds;
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

		GetMainCamera()->Update(deltaSeconds);
	}
}

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);

void cViewer::OnRender(const float deltaSeconds)
{
	GetMainLight().Bind(m_renderer, 0);

	m_gui.SetContext();
	m_gui.NewFrame();

	Matrix44 viewtoLightProj;

	{
		Vector3 lightPos;
		Matrix44 view, proj, tt;
		cLightManager::Get()->GetMainLight().GetShadowMatrix(
			Vector3(0, 0, 0), lightPos, view, proj, tt);

		Matrix44 mWVPT = view * proj * tt;

		viewtoLightProj = GetMainCamera()->GetViewMatrix().Inverse() * view * proj;

		for (auto &p : m_shaders)
		{
			m_shadowMap.Bind(*p, "g_shadowMapTexture");
			p->SetTechnique("ShadowMap");
			p->SetMatrix("g_mWVPT", mWVPT);
			p->SetMatrix("g_mView", view);
			p->SetMatrix("g_mProj", proj);
		}

		m_shadowMap.Begin(m_renderer);
		for (auto &p : m_models)
			p->RenderShader(m_renderer);
		m_shadowMap.End(m_renderer);
	}
	

	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();
		m_renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix44::Identity);

		m_skybox.Render(m_renderer);

		m_renderer.RenderGrid();
		m_renderer.RenderFPS();

		for (auto &p : m_shaders)
		{
			GetMainLight().Bind(*p);
			GetMainCamera()->Bind(*p);
		}

		const Vector3 lightPos = GetMainLight().GetPosition() * GetMainCamera()->GetViewMatrix();
		const Vector3 lightDir = GetMainLight().GetDirection().MultiplyNormal( GetMainCamera()->GetViewMatrix());

		for (auto &p : m_shaders)
			p->SetTechnique("Scene_ShadowMap");
		m_ground.RenderShader(m_renderer);
		for (auto &p : m_models)
		{
			if (p->m_shader)
			{
				p->m_shader->SetVector("g_vLightPos", lightPos);
				p->m_shader->SetVector("g_vLightDir", lightDir);
				p->m_shader->SetMatrix("g_mViewToLightProj", viewtoLightProj);
			}
			p->RenderShader(m_renderer);
		}

		m_shadowMap.RenderShadowMap(m_renderer);

		// Volume Shadow
		//for (auto &p : m_shaders)
		//	p->SetTechnique("Ambient");
		//m_ground.RenderShader(m_renderer);
		//for (auto &p : m_models)
		//	p->RenderShader(m_renderer);

		//if (m_isShadow)
		//{
		//	for (auto &p : m_shaders)
		//		p->SetTechnique("Shadow");
		//	for (auto &p : m_models)
		//		p->RenderShadow(m_renderer);
		//}

		//for (auto &p : m_shaders)
		//	p->SetTechnique("Scene");
		//m_ground.RenderShader(m_renderer);
		//for (auto &p : m_models)
		//	p->RenderShader(m_renderer);

		m_renderer.RenderAxis();
		m_gui.Render();
		m_renderer.EndScene();
		m_renderer.Present();
	}
}


void cViewer::OnDeviceLost()
{
	m_gui.InvalidateDeviceObjects();
	m_shadowMap.LostDevice();
	m_renderer.ResetDevice(0, 0, true);
	m_gui.CreateDeviceObjects();
	m_shadowMap.ResetDevice(m_renderer);
}


void cViewer::OnShutdown()
{
	m_gui.Shutdown();

	for (auto &p : m_models)
		delete p;
	m_models.clear();
	
}


void cViewer::ChangeWindowSize()
{
	if (m_renderer.CheckResetDevice())
	{
		m_gui.InvalidateDeviceObjects();
		m_shadowMap.LostDevice();
		m_renderer.ResetDevice();
		m_gui.CreateDeviceObjects();
		m_shadowMap.ResetDevice(m_renderer);
	}
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	cInputManager::Get()->MouseProc(message, wParam, lParam);

	m_gui.WndProcHandler(m_hWnd, message, wParam, lParam);

	if ((message != WM_EXITSIZEMOVE) && (message != WM_SIZE))
		if (ImGui::IsMouseHoveringAnyWindow())
			return;


	static bool maximizeWnd = false;
	switch (message)
	{
	case WM_EXITSIZEMOVE:
		ChangeWindowSize();
		break;

	case WM_SIZE:
		if (SIZE_MAXIMIZED == wParam)
		{
			maximizeWnd = true;
			ChangeWindowSize();
		}
		else if (maximizeWnd && (SIZE_RESTORED == wParam))
		{
			maximizeWnd = false;
			ChangeWindowSize();
		}
		break;

	case WM_DROPFILES:
	{
		const HDROP hdrop = (HDROP)wParam;
		char filePath[MAX_PATH];
		const UINT size = DragQueryFileA(hdrop, 0, filePath, MAX_PATH);
		if (size == 0)
			return;// handle error...
	}
	break;

	case WM_MOUSEWHEEL:
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

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

		case VK_RETURN:
			cResourceManager::Get()->ReloadShader(m_renderer);
			break;

		case VK_SPACE:
			m_isShadow = !m_isShadow;
			break;
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

			//graphic::GetMainCamera()->Yaw2(x * 0.005f);
			//graphic::GetMainCamera()->Pitch2(y * 0.005f);

			graphic::GetMainCamera()->Yaw3(x * 0.005f, Vector3(0, 0, 0));
			graphic::GetMainCamera()->Pitch3(y * 0.005f, Vector3(0, 0, 0));

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

