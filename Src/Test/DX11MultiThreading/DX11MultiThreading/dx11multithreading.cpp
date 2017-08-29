//
// DX11 Two Window
//

#include "../../../../../Common/Common/common.h"
using namespace common;
#include "../../../../../Common/Graphic11/graphic11.h"
#include "../../../../../Common/Framework11/framework11.h"


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
	cCamera m_terrainCamera;
	cCamera m_cam2;
	cGrid m_ground;
	cDbgArrow m_dbgArrow;
	cDbgAxis m_axis;
	cTexture m_texture;
	cImGui m_imgui;

	Transform m_world;
	cMaterial m_mtrl;
	
	HANDLE m_threadHandle;
	bool m_isThreadLoop = true;
	cRenderer m_renderer2;
	bool m_sync = false;

	//HWND m_hWnd2;
	//IDXGISwapChain *m_swapChain2;
	//ID3D11RenderTargetView *m_renderTargetView;
	//ID3D11Texture2D *m_depthStencil;
	//ID3D11DepthStencilView *m_depthStencilView;
	//ID3D11DeviceContext *m_deferredContext;
	//ID3D11CommandList *m_cmdLists = NULL;


	bool m_isFrustumTracking = true;
	sf::Vector2i m_curPos;
	Plane m_groundPlane1, m_groundPlane2;
	float m_moveLen;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);

LRESULT CALLBACK WndProc2(HWND, UINT, WPARAM, LPARAM);
unsigned int WINAPI RenderDeferredProc(LPVOID lpParameter);


cViewer::cViewer()
	: m_groundPlane1(Vector3(0, 1, 0), 0)
{
	m_windowName = L"Two Window";
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
	//SAFE_RELEASE(m_swapChain2);
	//SAFE_RELEASE(m_renderTargetView);
	//SAFE_RELEASE(m_depthStencil);
	//SAFE_RELEASE(m_depthStencilView);
	m_isThreadLoop = false;
	WaitForSingleObject(m_threadHandle, INFINITY);
	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	DragAcceptFiles(m_hWnd, TRUE);

	//cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = m_windowRect.right - m_windowRect.left;
	const int WINSIZE_Y = m_windowRect.bottom - m_windowRect.top;
	m_cam2.Init(&m_renderer);
	m_cam2.SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_cam2.SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 10000.0f);
	m_cam2.SetViewPort(WINSIZE_X, WINSIZE_Y);

	m_terrainCamera.Init(&m_renderer);
	m_terrainCamera.SetCamera(Vector3(-3, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_terrainCamera.SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.0f, 10000.f);
	m_terrainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 300, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_ground.Create(m_renderer, 10, 10, 1, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE | eVertexType::TEXTURE);
	m_ground.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	m_mtrl.InitWhite();

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_axis.Create(m_renderer);
	m_axis.SetAxis(bbox2, false);

	m_imgui.Init(m_hWnd, m_renderer.GetDevice(), m_renderer.GetDevContext());

	// Two Window
	HWND hWnd = framework::InitWindow(m_hInstance
		, L"Window2"
		, sRecti(0, 0, 1024, 768)
		, SW_NORMAL
		, WndProc2
		, false);


	//HRESULT hr = S_OK;
	//// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	//IDXGIFactory* dxgiFactory = nullptr;
	//{
	//	IDXGIDevice* dxgiDevice = nullptr;
	//	hr = m_renderer.GetDevice()->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	//	if (SUCCEEDED(hr))
	//	{
	//		IDXGIAdapter* adapter = nullptr;
	//		hr = dxgiDevice->GetAdapter(&adapter);
	//		if (SUCCEEDED(hr))
	//		{
	//			hr = adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
	//			adapter->Release();
	//		}
	//		dxgiDevice->Release();
	//	}
	//}

	//if (FAILED(hr))
	//	return E_FAIL;

	//DXGI_SWAP_CHAIN_DESC sd;
	//ZeroMemory(&sd, sizeof(sd));
	//sd.BufferCount = 1;
	//sd.BufferDesc.Width = 1024;
	//sd.BufferDesc.Height = 768;
	//sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//sd.BufferDesc.RefreshRate.Numerator = 60;
	//sd.BufferDesc.RefreshRate.Denominator = 1;
	////sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//sd.OutputWindow = m_hWnd2;
	//sd.SampleDesc.Count = 1;
	//sd.SampleDesc.Quality = 0;
	//sd.Windowed = TRUE;
	////sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//hr = dxgiFactory->CreateSwapChain(m_renderer.GetDevice(), &sd, &m_swapChain2);

	//// Create a render target view
	//ID3D11Texture2D* pBackBuffer = NULL;
	//hr = m_swapChain2->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	//if (FAILED(hr))
	//	return false;

	//hr = m_renderer.GetDevice()->CreateRenderTargetView(pBackBuffer, NULL, &m_renderTargetView);
	//pBackBuffer->Release();
	//if (FAILED(hr))
	//	return false;

	//// Create depth stencil texture
	//D3D11_TEXTURE2D_DESC descDepth;
	//ZeroMemory(&descDepth, sizeof(descDepth));
	//descDepth.Width = 1024;
	//descDepth.Height = 768;
	//descDepth.MipLevels = 1;
	//descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//descDepth.SampleDesc.Count = 1;
	//descDepth.SampleDesc.Quality = 0;
	//descDepth.Usage = D3D11_USAGE_DEFAULT;
	//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//descDepth.CPUAccessFlags = 0;
	//descDepth.MiscFlags = 0;
	//hr = m_renderer.GetDevice()->CreateTexture2D(&descDepth, NULL, &m_depthStencil);
	//if (FAILED(hr))
	//	return false;

	//// Create the depth stencil view
	//D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	//ZeroMemory(&descDSV, sizeof(descDSV));
	//descDSV.Format = descDepth.Format;
	//descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//descDSV.Texture2D.MipSlice = 0;
	//hr = m_renderer.GetDevice()->CreateDepthStencilView(m_depthStencil, &descDSV, &m_depthStencilView);
	//if (FAILED(hr))
	//	return false;

	//hr = m_renderer.GetDevice()->CreateDeferredContext(0, &m_deferredContext);
	//if (FAILED(hr))
	//	return false;

	if (!m_renderer2.CreateDirectX(false, hWnd, 1024, 768, m_renderer.GetDevice()))
		return false;

	m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, RenderDeferredProc, this, CREATE_SUSPENDED, 0);
	ResumeThread(m_threadHandle);

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	cAutoCam cam(&m_terrainCamera);

	GetMainCamera().Update(deltaSeconds);
}


bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);


void cViewer::OnRender(const float deltaSeconds)
{
	if (m_isFrustumTracking)
		cMainCamera::Get()->PushCamera(&m_terrainCamera);

	m_imgui.NewFrame();

	//GetMainLight().Bind(m_renderer, 0);
	m_renderer.m_textMgr.NewFrame();

	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_col);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	static bool show_test_window = true;
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
	ImGui::ShowTestWindow(&show_test_window);

	//if (m_renderer.ClearScene())
	//{
	//	m_renderer.BeginScene();

	//	Transform tfm1;
	//	tfm1.pos = Vector3(3, 2, 0);
	//	Transform tfm2;
	//	tfm2.pos = Vector3(-3, 2, 0);

	//	m_renderer.m_textMgr.AddTextRender(m_renderer, 12345, L"Test1", cColor::BLUE, cColor::BLACK
	//		, BILLBOARD_TYPE::Y_AXIS, tfm1);

	//	m_renderer.m_textMgr.AddTextRender(m_renderer, 12346, L"Test2", cColor::RED, cColor::BLACK
	//		, BILLBOARD_TYPE::Y_AXIS, tfm2);

	//	GetMainCamera().Bind(m_renderer);

	//	static float t = 0;
	//	t += deltaSeconds;

	//	XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetViewMatrix());
	//	XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetProjectionMatrix());
	//	XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());

	//	m_renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
	//	m_renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
	//	m_renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);

	//	m_renderer.m_cbLight = GetMainLight().GetLight();
	//	m_renderer.m_cbLight.Update(m_renderer, 1);
	//	m_renderer.m_cbMaterial = m_mtrl.GetMaterial();
	//	m_renderer.m_cbMaterial.Update(m_renderer, 2);

	//	m_ground.Render(m_renderer);
	//	m_axis.Render(m_renderer);
	//	m_renderer.RenderFPS();

	//	m_renderer.EndScene();
	//	m_imgui.Render();
	//	m_renderer.Present();
	//}
	cRenderer &renderer = m_renderer;
	cRenderer &renderer2 = m_renderer2;

	//if (renderer2.ClearScene())
	//{
	//	renderer2.SetRenderTarget(renderer.m_renderTargetView, renderer.m_depthStencilView);

	//	renderer2.BeginScene();

	//	Transform tfm1;
	//	tfm1.pos = Vector3(3, 2, 0);
	//	Transform tfm2;
	//	tfm2.pos = Vector3(-3, 2, 0);

	//	renderer2.m_textMgr.AddTextRender(renderer2, 12345, L"Test1", cColor::BLUE, cColor::BLACK
	//		, BILLBOARD_TYPE::Y_AXIS, tfm1);

	//	renderer2.m_textMgr.AddTextRender(renderer2, 12346, L"Test2", cColor::RED, cColor::BLACK
	//		, BILLBOARD_TYPE::Y_AXIS, tfm2);

	//	m_cam2.Bind(renderer2);

	//	//static float t = 0;
	//	//t += deltaSeconds;

	//	XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_cam2.GetViewMatrix());
	//	XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_cam2.GetProjectionMatrix());
	//	XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());

	//	renderer2.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
	//	renderer2.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
	//	renderer2.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);

	//	renderer2.m_cbLight = GetMainLight().GetLight();
	//	renderer2.m_cbLight.Update(renderer2, 1);
	//	renderer2.m_cbMaterial = m_mtrl.GetMaterial();
	//	renderer2.m_cbMaterial.Update(renderer2, 2);

	//	m_ground.Render(renderer2);
	//	m_axis.Render(renderer2);
	//	renderer2.RenderFPS();

	//	renderer2.EndScene();
	//	//m_imgui.Render();
	//	renderer2.FinishCommandList();
	//}

	while (!m_sync) {
		Sleep(1);
	}

	renderer2.FinishCommandList();

	// Render
	//m_renderer.GetDevContext()->OMSetRenderTargets(1
	//	, &m_renderer.m_renderTargetView, m_renderer.m_depthStencilView);
	//renderer.SetRenderTarget(renderer2.m_renderTargetView, renderer2.m_depthStencilView);
	//renderer.ExecuteCommandList(renderer2.m_cmdList);
	//SAFE_RELEASE(renderer2.m_cmdList);
	renderer.Present();
	//renderer.GetDevContext()->ClearState();
	m_sync = false;

	if (m_isFrustumTracking)
		cMainCamera::Get()->PopCamera();
}


void cViewer::OnLostDevice()
{
	m_renderer.ResetDevice(0, 0, true);
	m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
}


void cViewer::OnShutdown()
{
}


void cViewer::ChangeWindowSize()
{
	if (m_renderer.CheckResetDevice())
	{
		m_renderer.ResetDevice();
		m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
	}
}

void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_imgui.WndProcHandler(m_hWnd, message, wParam, lParam);

	framework::cInputManager::Get()->MouseProc(message, wParam, lParam);

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

		const float len = graphic::GetMainCamera().GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera().Zoom((zDelta<0) ? -zoomLen : zoomLen);

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
			//m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			//m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case VK_RETURN:
			//cResourceManager::Get()->ReloadShader(m_renderer);
			break;
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
		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
		m_moveLen = common::clamp(1, 100, (p1 - orig).Length());
		graphic::GetMainCamera().MoveCancel();

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
		if (m_isFrustumTracking)
			cMainCamera::Get()->PushCamera(&m_terrainCamera);

		sf::Vector2i pos = { (int)LOWORD(lParam), (int)HIWORD(lParam) };

		Vector3 orig, dir;
		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
		//m_line.SetLine(orig + Vector3(1, 0, 0), p1, 0.3f);
		//m_dbgLine.SetLine(orig + Vector3(-1, 0, 0), p1 + Vector3(-1, 0, 0), 0.3f);

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

			graphic::GetMainCamera().Yaw2(x * 0.005f);
			graphic::GetMainCamera().Pitch2(y * 0.005f);

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

		if (m_isFrustumTracking)
			cMainCamera::Get()->PopCamera();
	}
	break;
	}
}


LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//cGameMain::Get()->MessageProc(message, wParam, lParam);

	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case VK_RETURN:
			break;
		}
		break;

	case WM_DESTROY:
		//cGameMain::Get()->Exit();
		PostQuitMessage(0);
		break;

	case WM_ERASEBKGND:
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


unsigned int WINAPI RenderDeferredProc(LPVOID lpParameter)
{
	cViewer *p = (cViewer*)lpParameter;
	cRenderer &renderer = p->m_renderer;
	cRenderer &renderer2 = p->m_renderer2;

	while (p->m_isThreadLoop)
	{

		renderer2.SetRenderTarget(renderer.m_renderTargetView, renderer.m_depthStencilView);

		if (renderer2.ClearScene())
		{
			renderer2.BeginScene();

			Transform tfm1;
			tfm1.pos = Vector3(3, 2, 0);
			Transform tfm2;
			tfm2.pos = Vector3(-3, 2, 0);

			renderer2.m_textMgr.AddTextRender(renderer2, 12345, L"Test1", cColor::BLUE, cColor::BLACK
				, BILLBOARD_TYPE::Y_AXIS, tfm1);

			renderer2.m_textMgr.AddTextRender(renderer2, 12346, L"Test2", cColor::RED, cColor::BLACK
				, BILLBOARD_TYPE::Y_AXIS, tfm2);

			p->m_cam2.Bind(renderer2);

			//static float t = 0;
			//t += deltaSeconds;

			XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&p->m_cam2.GetViewMatrix());
			XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&p->m_cam2.GetProjectionMatrix());
			XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&p->m_world.GetMatrix());

			renderer2.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
			renderer2.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
			renderer2.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);

			renderer2.m_cbLight = GetMainLight().GetLight();
			renderer2.m_cbLight.Update(renderer2, 1);
			renderer2.m_cbMaterial = p->m_mtrl.GetMaterial();
			renderer2.m_cbMaterial.Update(renderer2, 2);

			p->m_ground.Render(renderer2);
			p->m_axis.Render(renderer2);
			renderer2.RenderFPS();

			renderer2.EndScene();
			//p->m_imgui.Render();
			//renderer2.FinishCommandList();

			p->m_sync = true;
			while (p->m_isThreadLoop && p->m_sync) {
				Sleep(1);
			}

		}
	}

	return 0;
}