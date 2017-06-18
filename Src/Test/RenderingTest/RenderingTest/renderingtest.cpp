//
// Rendering Test
//

#include "stdafx.h"
#include "wall.h"

using namespace graphic;


class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float deltaSeconds) override;
	virtual void OnRender(const float deltaSeconds) override;
	virtual void OnLostDevice() override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void ChangeWindowSize();


public:
	cImGui m_gui;
	cGrid3 m_ground;
	cSkyBox m_skybox;
	cTerrain2 m_terrain;
	cCamera m_terrainCamera;
	cModel2 *m_model;

	enum {FRUSTUM_COUNT=3};
	cDbgFrustum m_frustum[ FRUSTUM_COUNT];
	cDbgQuad2 m_quad[ FRUSTUM_COUNT];

	vector<cShader*> m_shaders;

	bool m_isShadow = true;
	bool m_isFrustumTracking = true;
	bool m_isCullingModel = true;
	bool m_isShowLightFrustum = false;
	bool m_isShowFrustum = true;
	bool m_isShowFrustumQuad = true;
	sf::Vector2i m_curPos;
	Plane m_groundPlane1, m_groundPlane2;
	float m_moveLen;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);


cViewer::cViewer()
	: m_groundPlane1(Vector3(0, 1, 0), 0)
{
	m_windowName = L"Rendering Test";
	//const RECT r = { 0, 0, 1024, 768 };
	const RECT r = { 0, 0, 1280, 1024};
	m_windowRect = r;
	m_moveLen = 0;
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

	const int WINSIZE_X = m_windowRect.right - m_windowRect.left;
	const int WINSIZE_Y = m_windowRect.bottom - m_windowRect.top;
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 10000.0f);
	GetMainCamera()->SetViewPort(WINSIZE_X, WINSIZE_Y);

	m_terrainCamera.Init(&m_renderer);
	m_terrainCamera.SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_terrainCamera.SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.0f, 10000.f);
	//m_terrainCamera.SetProjectionOrthogonal((float)WINSIZE_X, (float)WINSIZE_Y, 0.1f, 10000.0f);
	m_terrainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);
	

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 300, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	m_gui.Init(m_hWnd, m_renderer.GetDevice());
	m_gui.SetContext();

	m_ground.Create(m_renderer, 10, 10, 100, 200);
	m_ground.m_tex = cResourceManager::Get()->LoadTexture(m_renderer, "../media/terrain/¹Ù´Ú.jpg");

	m_skybox.Create(m_renderer, "skybox");

	for (int i = 0; i < FRUSTUM_COUNT; ++i)
	{
		m_frustum[i].Create(m_renderer, Matrix44::Identity);
		m_quad[i].Create(m_renderer);
	}

	//{
	//	string files[] = {
	//		"ChessBishop.x"
	//		, "ChessKing.x"
	//		, "ChessKnight.x"
	//		, "ChessPawn.x"
	//		, "ChessQueen.x"
	//		, "ChessRook.x"
	//	};
	//	const int size = sizeof(files) / sizeof(string);
	//	for (int k = 0; k < 0; ++k)
	//	{
	//		for (int i = 0; i < 0; ++i)
	//		{
	//			cModel2 *model = new cModel2();
	//			model->Create(m_renderer, files[i%size], "", "", true, false);

	//			Matrix44 T;
	//			T.SetPosition(Vector3(k*2.f, 0, i * 2.f) - Vector3(30,0,0));
	//			Matrix44 S;
	//			S.SetScale(Vector3(1, 1, 1) * 30);
	//			model->m_tm = S * T;			

	//			m_models.push_back(model);
	//		}		
	//	}
	//}

	{
		string files[] = {
			//"collada_skin.fx"
			// "collada_rigid.fx"
			"xfile.fx"
			, "cube3.fx"
			, "wall.fx"
		};
		const int size = sizeof(files) / sizeof(string);
		for (int i = 0; i < size; ++i)
		{
			cShader *p = cResourceManager::Get()->LoadShader(m_renderer, files[i]);
			if (p)
				m_shaders.push_back(p);
		}
		m_ground.SetShader(cResourceManager::Get()->LoadShader(m_renderer, files[1]));	
	}

	m_terrain.Create(m_renderer, sRectf(0, 500, 500, 0));

	const float size = 200;
	for (int tx = 0; tx < 1; ++tx)
	{
		for (int ty = 0; ty < 1; ++ty)
		{
			cTile *tile = new cTile();
			tile->Create(m_renderer, 
				common::format("%d-%d", tx, ty), 
				sRectf(tx*size, ty * size, tx * size + size, ty * size + size), 0.01f, 100.f
				, Vector2(0,0), Vector2(1, 1));
			tile->m_ground.m_tex = cResourceManager::Get()->LoadTexture(m_renderer, "terrain/¹Ù´Ú.jpg");

			const int xSize = 15;
			const int ySize = 15;
			for (int i = 0; i < xSize; ++i)
			{
				for (int k = 0; k < ySize; ++k)
				{
					cModel2 *model = new cModel2();
					model->Create(m_renderer, common::GenerateId(), "ConveyerBelt.x", "", "", true);
					//model->Create(m_renderer, common::GenerateId(), "bigship1.x", "", "", true);
					Matrix44 T;
					const float xGap = size / (xSize - 1);
					const float yGap = size / (ySize - 1);
					T.SetPosition(Vector3(tx* 50.f + k*xGap, 0, ty * 50.f + i * yGap));
					Matrix44 S;
					//S.SetScale(Vector3(1, 1, 1) * 20);
					S.SetScale(Vector3(1, 1, 1) * 0.03f);
					model->m_tm = S * T;

					model->SetShader(cResourceManager::Get()->LoadShader(m_renderer, "shader/xfile.fx"));
					tile->AddModel(model);
					m_model = model;
				}

				if (tx == 0 && ty == 0)
				{
					const Vector3 p1 = Vector3(tx*size+20, 0, ty * size + 100);
					{
						warehouse::cWall *wall = new warehouse::cWall;
						wall->Create(m_renderer, p1, p1 + Vector3(100, 0, 0), 10, 0.5f);
						tile->AddModel(wall);
					}
					{
						warehouse::cWall *wall = new warehouse::cWall;
						wall->Create(m_renderer, p1, p1 + Vector3(0, 0, 100), 10, 0.5f);
						tile->AddModel(wall);
					}
					{
						warehouse::cWall *wall = new warehouse::cWall;
						wall->Create(m_renderer, p1, p1 + Vector3(100, -1, 100), 1.1f, 0.5f, false);
						wall->m_shadowEpsilon = 0.001f;
						tile->AddModel(wall);
					}

					
				}

				m_terrain.AddTile(tile);
			}
		}
	}

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	m_terrain.Update(m_renderer, deltaSeconds);

	cFrustum::Split3(m_terrainCamera, 0.005f, 0.015f, 0.04f
		, m_frustum[0], m_frustum[1], m_frustum[2]);

	// keyboard
	if (GetFocus() == m_hWnd)
	{
		const float vel = 10 * deltaSeconds;
		//if (GetAsyncKeyState('W'))
		//	GetMainCamera()->MoveFront(vel);
		//else if (GetAsyncKeyState('A'))
		//	GetMainCamera()->MoveRight(-vel);
		//else if (GetAsyncKeyState('D'))
		//	GetMainCamera()->MoveRight(vel);
		//else if (GetAsyncKeyState('S'))
		//	GetMainCamera()->MoveFront(-vel);
		//else if (GetAsyncKeyState('E'))
		//	GetMainCamera()->MoveUp(vel);
		//else if (GetAsyncKeyState('C'))
		//	GetMainCamera()->MoveUp(-vel);


		if (m_model->m_xModel)
		{
			const int numFaces = m_model->m_xModel->m_pmesh->GetNumFaces();
			if (GetAsyncKeyState('A'))
			{
				m_model->m_xModel->m_pmesh->SetNumFaces(numFaces + 1);
				if (m_model->m_xModel->m_pmesh->GetNumFaces() == numFaces)
					m_model->m_xModel->m_pmesh->SetNumFaces(numFaces + 2);
			}

			if (GetAsyncKeyState('S'))
			{
				m_model->m_xModel->m_pmesh->SetNumFaces(numFaces - 1);
				if (m_model->m_xModel->m_pmesh->GetNumFaces() == numFaces)
					m_model->m_xModel->m_pmesh->SetNumFaces(numFaces - 2);
			}
		}

		GetMainCamera()->Update(deltaSeconds);
	}
}

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);

void cViewer::OnRender(const float deltaSeconds)
{
	if (m_isFrustumTracking)
		cMainCamera::Get()->PushCamera(&m_terrainCamera);

	GetMainLight().Bind(m_renderer, 0);

	//ParallelApplyFoo(*this, FRUSTUM_COUNT);

	for (int i = 0; i < FRUSTUM_COUNT; ++i)
	{
		m_terrain.CullingTest(m_renderer, m_frustum[i], m_isCullingModel, i);
		m_terrain.PreRender(m_renderer, Matrix44::Identity, i);
	}


	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();
		m_renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix44::Identity);

		GetMainCamera()->Bind(m_renderer);
		m_skybox.Render(m_renderer);
		m_renderer.RenderFPS();
		for (auto &p : m_shaders)
		{
			GetMainLight().Bind(*p);
			GetMainCamera()->Bind(*p);
		}

		Vector3 vtxBox[4];
		const Plane ground(Vector3(0, 1, 0), 0);
		for (int i = 0; i < FRUSTUM_COUNT; ++i)
		{
			m_frustum[i].GetGroundPlaneVertices(ground, vtxBox);
			if (m_isShowFrustumQuad)
				m_quad[i].SetQuad(vtxBox, 0.1f);
		}

		m_terrain.CullingTestOnly(m_renderer, m_terrainCamera, m_isCullingModel);
		m_terrain.Render(m_renderer);

		if (m_isShowFrustumQuad)
			for (int i=0; i < FRUSTUM_COUNT; ++i)
				m_quad[i].Render(m_renderer);

		if (m_isShowFrustum)
			for (int i = 0; i < FRUSTUM_COUNT; ++i)
				m_frustum[i].RenderShader(m_renderer);

		if (m_isShowLightFrustum)
			for (int i = 0; i < FRUSTUM_COUNT; ++i)
				m_terrain.m_dbgLightFrustum[i].RenderShader(m_renderer);

		//m_renderer.RenderAxis();
		m_renderer.EndScene();
		m_renderer.Present();
	}

	if (m_isFrustumTracking)
		cMainCamera::Get()->PopCamera();
}


void cViewer::OnLostDevice()
{
	m_gui.InvalidateDeviceObjects();
	m_terrain.LostDevice();
	m_renderer.ResetDevice(0, 0, true);
	m_gui.CreateDeviceObjects();
	m_terrain.ResetDevice(m_renderer);
	m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
}


void cViewer::OnShutdown()
{
	m_gui.Shutdown();
}


void cViewer::ChangeWindowSize()
{
	if (m_renderer.CheckResetDevice())
	{
		m_gui.InvalidateDeviceObjects();
		m_renderer.ResetDevice();
		m_gui.CreateDeviceObjects();
		m_terrain.ResetDevice(m_renderer);
		m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
		m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	}
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	framework::cInputManager::Get()->MouseProc(message, wParam, lParam);

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
		if (m_isFrustumTracking)
			cMainCamera::Get()->PushCamera(&m_terrainCamera);

		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera()->GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera()->Zoom((zDelta<0) ? -zoomLen : zoomLen);

		if (m_isFrustumTracking)
			cMainCamera::Get()->PopCamera();
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

		case VK_SPACE: m_isShadow = !m_isShadow; break;
		case '1': m_isShowLightFrustum = !m_isShowLightFrustum; break;
		case '2': 
		{
			// Switching Camera Option
			if (m_isFrustumTracking)
			{
				GetMainCamera()->SetEyePos(m_terrainCamera.GetEyePos());
				GetMainCamera()->SetLookAt(m_terrainCamera.GetLookAt());
			}
			else
			{
				m_terrainCamera.SetEyePos(GetMainCamera()->GetEyePos());
				m_terrainCamera.SetLookAt(GetMainCamera()->GetLookAt());
			}
			m_isFrustumTracking = !m_isFrustumTracking;
		}
		break;

		case '3': m_isCullingModel = !m_isCullingModel;  break;
		case '4': {
			static bool isDbgRender = false;
			isDbgRender = !isDbgRender;
			m_terrain.SetDbgRendering(isDbgRender);
		}
		break;
		case '5': m_isShowFrustum = !m_isShowFrustum; break;
		case '6': m_isShowFrustumQuad = !m_isShowFrustumQuad; break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		if (m_isFrustumTracking)
			cMainCamera::Get()->PushCamera(&m_terrainCamera);

		POINT pos = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);

		Vector3 orig, dir;
		graphic::GetMainCamera()->GetRay(pos.x, pos.y, orig, dir);
		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
		m_moveLen = common::clamp(1, 100, (p1 - orig).Length());
		graphic::GetMainCamera()->MoveCancel();

		if (m_isFrustumTracking)
			cMainCamera::Get()->PopCamera();
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
		SetCapture(m_hWnd);
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		ReleaseCapture();
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (m_isFrustumTracking)
			cMainCamera::Get()->PushCamera(&m_terrainCamera);

		sf::Vector2i pos = { (int)LOWORD(lParam), (int)HIWORD(lParam) };

		if (wParam & 0x10) // middle button down
		{
		}

		if (m_LButtonDown)
		{
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;

			if ((abs(x) > 1000) || (abs(y) > 1000))
			{
				if (m_isFrustumTracking)
					cMainCamera::Get()->PopCamera();
				break;
			}

			m_curPos = pos;

			Vector3 dir = graphic::GetMainCamera()->GetDirection();
			Vector3 right = graphic::GetMainCamera()->GetRight();
			dir.y = 0;
			dir.Normalize();
			right.y = 0;
			right.Normalize();

			graphic::GetMainCamera()->MoveRight(-x * m_moveLen * 0.001f);
			graphic::GetMainCamera()->MoveFrontHorizontal(y * m_moveLen * 0.001f);
		}
		else if (m_RButtonDown)
		{
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			graphic::GetMainCamera()->Yaw2(x * 0.005f);
			graphic::GetMainCamera()->Pitch2(y * 0.005f);

		}
		else if (m_MButtonDown)
		{
			const sf::Vector2i point = { pos.x - m_curPos.x, pos.y - m_curPos.y };
			m_curPos = pos;

			const float len = graphic::GetMainCamera()->GetDistance();
			graphic::GetMainCamera()->MoveRight(-point.x * len * 0.001f);
			graphic::GetMainCamera()->MoveUp(point.y * len * 0.001f);
		}
		else
		{
		}

		if (m_isFrustumTracking)
			cMainCamera::Get()->PopCamera();
	}
	break;
	}
}

