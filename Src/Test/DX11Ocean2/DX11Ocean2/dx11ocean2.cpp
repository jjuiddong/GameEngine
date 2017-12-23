//
// DX11 Ocean2 (Nvidia Island)
//

#include "stdafx.h"
#include "terrain.h"

using namespace graphic;

float g_BackBufferWidth = 1280.0f;
float g_BackBufferHeight = 720.0f;

int g_CurrentFrame = 0;
float g_FrameTime = 0;
float g_TotalTime = 0;

bool g_RenderWireframe = false;

float g_DynamicTessellationFactor = 50.0f;
float g_StaticTessellationFactor = 12.0f;

bool g_UseDynamicLOD = true;
bool g_RenderCaustics = true;
bool g_FrustumCullInHS = true;


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
	cMaterial m_mtrl;

	cShader11 m_shader;
	CTerrain g_Terrain;

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
	, m_mainCamera("main")
{
	m_windowName = L"DX11 Ocean2";
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
	dbg::RemoveLog();
	dbg::RemoveErrLog();

	DragAcceptFiles(m_hWnd, TRUE);
	cAutoCam cam(&m_mainCamera);

	cResourceManager::Get()->SetMediaDirectory("../media/");

	const float WINSIZE_X = (float)(m_windowRect.right - m_windowRect.left);
	const float WINSIZE_Y = (float)(m_windowRect.bottom - m_windowRect.top);
	m_mainCamera.SetCamera(Vector3(-100, 100, -100), Vector3(0, 0, 0), Vector3(0, 1, 0));
	float aspectRatio = WINSIZE_X / WINSIZE_Y;
	m_mainCamera.SetProjection(camera_fov * MATH_PI / 360.0f, aspectRatio, scene_z_near, scene_z_far);
	m_mainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 400, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_mtrl.InitWhite();

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_shader.Create(m_renderer, "../media/ocean2/Island11.fxo", "RenderSky", layout, 2);

	g_Terrain.MultiSampleCount = 4;// min(4, pDeviceSettings->d3d11.sd.SampleDesc.Count);
	g_Terrain.MultiSampleQuality = 0;

	// Initialize terrain 
	g_BackBufferWidth = WINSIZE_X;
	g_BackBufferHeight = WINSIZE_Y;

	g_Terrain.Initialize(m_renderer.GetDevice(), m_shader.m_effect);
	g_Terrain.LoadTextures();
	g_Terrain.BackbufferWidth = g_BackBufferWidth;
	g_Terrain.BackbufferHeight = g_BackBufferHeight;
	g_Terrain.ReCreateBuffers();

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	cAutoCam cam(&m_mainCamera);
	GetMainCamera().Update(deltaSeconds);
}


float g_LightPosition[3] = { -10000.0f,6500.0f,10000.0f };

void cViewer::OnRender(const float deltaSeconds)
{
	cAutoCam cam(&m_mainCamera);

	GetMainLight().Bind(m_renderer);
	GetMainCamera().Bind(m_renderer);

	CommonStates states(m_renderer.GetDevice());
	m_renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	m_renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
	m_renderer.GetDevContext()->OMSetBlendState(states.Opaque(), 0, 0xffffffff);
	//m_renderer.GetDevContext()->RSSetState(states.Wireframe());

	g_TotalTime += deltaSeconds;
	g_FrameTime = deltaSeconds;


	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();

		Vector2 WaterTexcoordShift(g_TotalTime*1.5f, g_TotalTime*0.75f);
		Vector2 ScreenSizeInv(1.0f / (g_BackBufferWidth*main_buffer_size_multiplier), 1.0f / (g_BackBufferHeight*main_buffer_size_multiplier));

		ID3DX11Effect *effect = m_shader.m_effect;
		effect->GetVariableByName("g_ZNear")->AsScalar()->SetFloat(scene_z_near);
		effect->GetVariableByName("g_ZFar")->AsScalar()->SetFloat(scene_z_far);
		effect->GetVariableByName("g_LightPosition")->AsVector()->SetFloatVector(g_LightPosition);
		effect->GetVariableByName("g_WaterBumpTexcoordShift")->AsVector()->SetFloatVector((float*)&WaterTexcoordShift);
		effect->GetVariableByName("g_ScreenSizeInv")->AsVector()->SetFloatVector((float*)&ScreenSizeInv);

		effect->GetVariableByName("g_DynamicTessFactor")->AsScalar()->SetFloat(g_DynamicTessellationFactor);
		effect->GetVariableByName("g_StaticTessFactor")->AsScalar()->SetFloat(g_StaticTessellationFactor);
		effect->GetVariableByName("g_UseDynamicLOD")->AsScalar()->SetFloat(g_UseDynamicLOD ? 1.0f : 0.0f);
		effect->GetVariableByName("g_RenderCaustics")->AsScalar()->SetFloat(g_RenderCaustics ? 1.0f : 0.0f);
		effect->GetVariableByName("g_FrustumCullInHS")->AsScalar()->SetFloat(g_FrustumCullInHS ? 1.0f : 0.0f);

		g_Terrain.Render(m_renderer, &GetMainCamera());

		m_renderer.RenderFPS();
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

		g_BackBufferWidth = m_renderer.m_viewPort.GetWidth();
		g_BackBufferHeight = m_renderer.m_viewPort.GetHeight();

		g_Terrain.BackbufferWidth = m_renderer.m_viewPort.GetWidth();
		g_Terrain.BackbufferHeight = m_renderer.m_viewPort.GetHeight();
		g_Terrain.ReCreateBuffers();
	}
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
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
		cAutoCam cam(&m_mainCamera);

		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera().GetDistance();
		float zoomLen = (len > 100) ? 10 : (len / 10.f);
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
			ID3D11Resource *texture;
			m_renderer.m_renderTargetView->GetResource(&texture);
			DirectX::SaveWICTextureToFile(m_renderer.GetDevContext(), texture, GUID_ContainerFormatPng, L"backbuff.png");
			texture->Release();
		}
		break;

		case '1': g_UseDynamicLOD = !g_UseDynamicLOD;  break;
		case '2': g_RenderCaustics = !g_RenderCaustics;  break;
		case '3': g_FrustumCullInHS = !g_FrustumCullInHS;  break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		cAutoCam cam(&m_mainCamera);

		POINT pos = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);

		const Ray ray = GetMainCamera().GetRay(pos.x, pos.y);
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

		cCamera *camera = &m_mainCamera;
		cAutoCam cam(camera);

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
	}
	break;
	}
}

