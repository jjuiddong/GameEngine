//
// DX11 Ocean
//

#include "stdafx.h"
#include "skybox.h"
#include "CFW1StateSaver.h"
#include "ocean_simulator.h"

using namespace graphic;


extern void renderShaded(cRenderer &renderer
	, const cCamera& camera
	, ID3D11ShaderResourceView* displacemnet_map
	, ID3D11ShaderResourceView* gradient_map
	, float time
	, ID3D11DeviceContext* pd3dContext);

extern void renderWireframe(cRenderer &renderer
	, const cCamera& camera
	, ID3D11ShaderResourceView* displacemnet_map
	, float time
	, ID3D11DeviceContext* pd3dContext);

extern void renderLogo(ID3D11DeviceContext* pd3dContext);


extern void initRenderResource(const ocean_parameter& ocean_param, ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);

extern Vector3 g_WaterbodyColor;
extern Vector3 g_SkyColor;


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
	cCamera3D m_mainCamera;
	cCamera3D m_secondCamera;
	cGrid m_ground;
	cDbgAxis m_axis;
	cTexture m_texture;

	cCamera3D m_lightCamera;
	cDbgFrustum m_dbgLightFrustum;
	cDbgFrustum m_dbgMainFrustum;

	cText m_text[6];
	CSkybox11 m_skybox;
	ID3D11Texture2D* m_pSkyCubeMap = NULL;
	ID3D11ShaderResourceView* m_pSRV_SkyCube = NULL;
	ocean_simulator* g_pocean_simulator = NULL;

	cImGui m_gui;

	bool m_showSecondCam = false;
	bool m_showMainFrustum = false;
	bool m_showLightFrustum1 = false;
	bool m_showLightFrustum2 = false;
	bool m_showCascade = false;
	bool m_showShadowMap = false;
	bool m_isShowWireFrame = false;

	cMaterial m_mtrl;
	bool m_isShadowRender = true;
	bool m_isMoveLightCamera = false;

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
	, m_mainCamera("main camera")
	, m_secondCamera("second camera")
	, m_lightCamera("light camera")
{
	m_windowName = L"DX11 Ocean";
	//const RECT r = { 0, 0, 1024, 768 };
	const RECT r = { 0, 0, 1280, 1024 };
	m_windowRect = r;
	m_moveLen = 0;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	m_gui.Shutdown();
	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	dbg::RemoveLog();
	dbg::RemoveErrLog();

	DragAcceptFiles(m_hWnd, TRUE);
	cAutoCam cam(&m_mainCamera);

	cResourceManager::Get()->SetMediaDirectory("../media/");

	const float WINSIZE_X = (float)(m_windowRect.right - m_windowRect.left);
	const float WINSIZE_Y = (float)(m_windowRect.bottom - m_windowRect.top);

	m_mainCamera.SetCamera(Vector3(-1000, 1000, -1000), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_mainCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.f, 100000.f);
	m_mainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	m_secondCamera.SetCamera(Vector3(-10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_secondCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.0f, 10000.f);
	m_secondCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 400, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_mtrl.InitWhite();

	m_ground.Create(m_renderer, 10, 10, 100, 100, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE0
		, cColor::WHITE
		//, g_defaultTexture
		, "tile.jpg"
		, Vector2(0, 0), Vector2(1, 1), 256
	);
	//m_ground.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	m_texture.Create(m_renderer, "../media/whitetex.dds");

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_axis.Create(m_renderer);
	m_axis.SetAxis(bbox2, false);

	//m_model.Create(m_renderer, common::GenerateId(), "../media/boxlifter.x");

	m_lightCamera.SetCamera(lightPos, lightLookat, Vector3(0, 1, 0));
	m_lightCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.0f, 10000.f);
	m_lightCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	cViewport svp = m_renderer.m_viewPort;
	svp.m_vp.MinDepth = 0.f;
	svp.m_vp.MaxDepth = 1.f;
	svp.m_vp.Width = 1024;
	svp.m_vp.Height = 1024;
	//m_shadowMap.Create(m_renderer, svp, DXGI_FORMAT_R32_FLOAT);
	m_dbgLightFrustum.Create(m_renderer, m_lightCamera.GetViewProjectionMatrix());
	m_dbgLightFrustum.SetFrustum(m_renderer, m_dbgLightFrustum.m_viewProj);

	m_dbgMainFrustum.Create(m_renderer, m_mainCamera.GetViewProjectionMatrix());
	m_dbgMainFrustum.SetFrustum(m_renderer, m_dbgMainFrustum.m_viewProj);

	for (int i = 0; i < 6; ++i)
		m_text[i].Create(m_renderer, 18, false, "Arial", cColor::BLACK);

	const Vector3 quadVtx[] = {
		Vector3(-5,0,5)
		, Vector3(5,0,5)
		, Vector3(5,0,-5)
		, Vector3(-5,0,-5)
	};

	
	CreateDDSTextureFromFile(m_renderer.GetDevice(),
		L"../media/ocean/sky_cube.dds", NULL, &m_pSRV_SkyCube);
	assert(m_pSRV_SkyCube);

	m_pSRV_SkyCube->GetResource((ID3D11Resource**)&m_pSkyCubeMap);
	assert(m_pSkyCubeMap);

	m_skybox.OnD3D11CreateDevice(m_renderer.GetDevice(), 50, m_pSkyCubeMap, m_pSRV_SkyCube);

	DXGI_SURFACE_DESC surfaceDesc;
	surfaceDesc.Width = (UINT)WINSIZE_X;
	surfaceDesc.Height = (UINT)WINSIZE_Y;
	m_skybox.OnD3D11ResizedSwapChain(&surfaceDesc);

	ocean_parameter ocean_param;
	g_pocean_simulator = new ocean_simulator(ocean_param, m_renderer.GetDevice());

	// Update the simulation for the first time.
	g_pocean_simulator->update_displacement_map(0);

	D3D11_RENDER_TARGET_VIEW_DESC backBufferDesc;
	m_renderer.m_renderTargetView->GetDesc(&backBufferDesc);
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	m_renderer.m_swapChain->GetDesc(&swapChainDesc);

	DXGI_SURFACE_DESC desc;
	desc.Width = (UINT)m_renderer.m_viewPort.GetWidth();
	desc.Height = (UINT)m_renderer.m_viewPort.GetHeight();
	desc.Format = backBufferDesc.Format;
	desc.SampleDesc = swapChainDesc.SampleDesc;
	initRenderResource(ocean_param, m_renderer.GetDevice(), &desc);


	m_gui.Init(m_hWnd, m_renderer.GetDevice(), m_renderer.GetDevContext(), NULL);
	m_gui.SetContext();

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	cAutoCam cam(&m_mainCamera);
	GetMainCamera().Update(deltaSeconds);

	static double app_time = 0;
	app_time += (double)deltaSeconds;
	g_pocean_simulator->update_displacement_map((float)app_time);
}


void cViewer::OnRender(const float deltaSeconds)
{
	cAutoCam cam(m_showSecondCam ? &m_secondCamera : &m_mainCamera);

	m_gui.NewFrame();

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::DragFloat("choppy scale", &g_pocean_simulator->m_param.choppy_scale, 0.01f);
	ImGui::ColorPicker3("Ocean Color", (float*)&g_WaterbodyColor);
	ImGui::ColorPicker3("Sky Color", (float*)&g_SkyColor);

	GetMainLight().Bind(m_renderer);
	GetMainCamera().Bind(m_renderer);

	CommonStates states(m_renderer.GetDevice());

	m_renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	m_renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
	m_renderer.GetDevContext()->OMSetBlendState(states.Opaque(), 0, 0xffffffff);

	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();

		Matrix44 view = Matrix44(1, 0, 0, 0
								, 0, 0, 1, 0
								, 0, 1, 0, 0
								, 0, 0, 0, 1) * GetMainCamera().GetViewMatrix();
		Matrix44 proj = GetMainCamera().GetProjectionMatrix();
		Matrix44 viewProj = view * proj;

		m_skybox.D3D11Render(&viewProj, m_renderer.GetDevContext());


		static double app_time = deltaSeconds;
		static double app_prev_time = deltaSeconds;
		app_time += deltaSeconds - app_prev_time;
		app_prev_time = deltaSeconds;

		ID3D11ShaderResourceView* tex_displacement = g_pocean_simulator->get_direct3d_displacement_map();
		ID3D11ShaderResourceView* tex_gradient = g_pocean_simulator->get_direct3d_gradient_map();

		if (m_isShowWireFrame)
			renderWireframe(m_renderer, GetMainCamera(), tex_displacement, (float)app_time, m_renderer.GetDevContext());
		else
			renderShaded(m_renderer, GetMainCamera(), tex_displacement, tex_gradient, (float)app_time, m_renderer.GetDevContext());


		m_gui.Render();

		FW1FontWrapper::CFW1StateSaver stateSaver;
		stateSaver.saveCurrentState(m_renderer.GetDevContext());
		m_renderer.RenderFPS();
		stateSaver.restoreSavedState();

		m_renderer.EndScene();
		m_renderer.Present();
	}
}


void cViewer::OnLostDevice()
{
	m_renderer.ResetDevice(0, 0, true);
	m_mainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
}


void cViewer::OnShutdown()
{
}


void cViewer::ChangeWindowSize()
{
	if (m_renderer.CheckResetDevice())
	{
		m_renderer.ResetDevice();
		m_mainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
	}
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_gui.WndProcHandler(m_hWnd, message, wParam, lParam);

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
		//cMainCamera::Get()->PushCamera(&m_mainCamera);
		cAutoCam cam(m_showSecondCam ? &m_secondCamera : &m_mainCamera);

		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera().GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera().Zoom((zDelta<0) ? -zoomLen : zoomLen);
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
		{
			static bool flag = false;
			//m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			//m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case VK_RETURN:
		{
			m_isShadowRender = !m_isShadowRender;

			ID3D11Resource *texture;
			m_renderer.m_renderTargetView->GetResource(&texture);
			DirectX::SaveWICTextureToFile(m_renderer.GetDevContext(), texture, GUID_ContainerFormatPng, L"backbuff.png");
			texture->Release();
		}
		break;

		case VK_SPACE:
			m_isShowWireFrame = !m_isShowWireFrame;
			break;

		case '1': m_showSecondCam = !m_showSecondCam; break;
		case '2': m_showMainFrustum = !m_showMainFrustum; break;
		case '3': m_showLightFrustum1 = !m_showLightFrustum1; break;
		case '4': m_showLightFrustum2 = !m_showLightFrustum2; break;
		case '5': m_showCascade = !m_showCascade; break;
		case '6': m_showShadowMap = !m_showShadowMap; break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		cAutoCam cam(m_showSecondCam ? &m_secondCamera : &m_mainCamera);

		POINT pos = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);

		const Ray ray = graphic::GetMainCamera().GetRay(pos.x, pos.y);
		Vector3 p1 = m_groundPlane1.Pick(ray.orig, ray.dir);
		m_moveLen = common::clamp(1, 100, (p1 - ray.orig).Length());
		graphic::GetMainCamera().MoveCancel();
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
			GetMainCamera().GetProjectionMatrix(),
			GetMainCamera().GetViewMatrix());
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
		const bool isPressCtrl = GetAsyncKeyState(VK_LCONTROL) ? true : false;

		cCamera *camera = m_showSecondCam ? &m_secondCamera : &m_mainCamera;
		cAutoCam cam(isPressCtrl ? &m_lightCamera : camera);

		sf::Vector2i pos = { (int)LOWORD(lParam), (int)HIWORD(lParam) };

		const Ray ray = GetMainCamera().GetRay(pos.x, pos.y);
		Vector3 p1 = m_groundPlane1.Pick(ray.orig, ray.dir);

		if (wParam & 0x10) // middle button down
		{
		}

		if (m_LButtonDown)
		{
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;

			if ((abs(x) > 1000) || (abs(y) > 1000))
			{
				//cMainCamera::Get()->PopCamera();
				break;
			}

			m_curPos = pos;

			Vector3 dir = graphic::GetMainCamera().GetDirection();
			Vector3 right = graphic::GetMainCamera().GetRight();
			dir.y = 0;
			dir.Normalize();
			right.y = 0;
			right.Normalize();

			graphic::GetMainCamera().MoveRight(-x * m_moveLen * 0.001f);
			graphic::GetMainCamera().MoveFrontHorizontal(y * m_moveLen * 0.001f);
		}
		else if (m_RButtonDown)
		{
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			m_mainCamera.Yaw2(x * 0.005f);
			m_mainCamera.Pitch2(y * 0.005f);

		}
		else if (m_MButtonDown)
		{
			const sf::Vector2i point = { pos.x - m_curPos.x, pos.y - m_curPos.y };
			m_curPos = pos;

			const float len = graphic::GetMainCamera().GetDistance();
			graphic::GetMainCamera().MoveRight(-point.x * len * 0.001f);
			graphic::GetMainCamera().MoveUp(point.y * len * 0.001f);
		}
		else
		{
		}

		if (isPressCtrl)
			m_dbgLightFrustum.SetFrustum(m_renderer, m_lightCamera.GetViewProjectionMatrix());

		m_dbgMainFrustum.SetFrustum(m_renderer, m_mainCamera.GetViewProjectionMatrix());
	}
	break;
	}
}

