//
// DX11 DockWindow
//

#include "../../../../../Common/Common/common.h"
using namespace common;
#include "../../../../../Common/Graphic11/graphic11.h"
#include "../../../../../Common/Framework11/framework11.h"


using namespace graphic;
using namespace framework;

class cViewer : public framework::cGameMain2
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float deltaSeconds) override;
	virtual void OnRender(const float deltaSeconds) override;
	//virtual void OnLostDevice() override;
	//virtual void OnShutdown() override;
	//virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual void OnEventProc(const sf::Event &evt) override;
	//void ChangeWindowSize();


public:
	cCamera m_terrainCamera;
	cGrid m_ground;
	cDbgArrow m_dbgArrow;
	cDbgAxis m_axis;
	cTexture m_texture;

	Transform m_world;
	cMaterial m_mtrl;

	bool m_isFrustumTracking = true;
	sf::Vector2i m_curPos;
	Plane m_groundPlane1, m_groundPlane2;
	float m_moveLen;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK3(cViewer);


class cDockView1 : public framework::cDockWindow
{
public:
	cDockView1(const string &name) : framework::cDockWindow(name) {
	}
	virtual ~cDockView1() {
	}

	bool Init() {
		return true;
	}
	virtual void OnUpdate(const float deltaSeconds) override {

	}
	virtual void OnRender() override {
	}
	virtual void OnPreRender() override {

	}
	virtual void OnPostRender() override {

	}
	virtual void OnResizeEnd(const framework::eDockResize::Enum type, const sRectf &rect) override {

	}
};




cViewer::cViewer()
	: m_groundPlane1(Vector3(0, 1, 0), 0)
{
	m_windowName = L"DX11 DockWindow";
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
	//graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	DragAcceptFiles(m_hWnd, TRUE);

	//cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = m_windowRect.right - m_windowRect.left;
	const int WINSIZE_Y = m_windowRect.bottom - m_windowRect.top;
	GetMainCamera().Init(&m_renderer);
	GetMainCamera().SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera().SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 10000.0f);
	GetMainCamera().SetViewPort(WINSIZE_X, WINSIZE_Y);

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

	m_gui.SetContext();

	cDockView1 *view1 = new cDockView1("DockView1");
	view1->Create(eDockState::DOCK, eDockSlot::TAB, this, NULL);
	view1->Init();

	cDockView1 *view2 = new cDockView1("DockView2");
	view2->Create(eDockState::DOCK, eDockSlot::BOTTOM, this, view1);
	view2->Init();

	cDockView1 *view3 = new cDockView1("DockView3");
	view3->Create(eDockState::DOCK, eDockSlot::RIGHT, this, view2);
	view3->Init();

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	__super::OnUpdate(deltaSeconds);

	cAutoCam cam(&m_terrainCamera);

	GetMainCamera().Update(deltaSeconds);
}


bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);


void cViewer::OnRender(const float deltaSeconds)
{
	//if (m_isFrustumTracking)
	//	cMainCamera::Get()->PushCamera(&m_terrainCamera);

	////GetMainLight().Bind(m_renderer, 0);
	//m_renderer.m_textMgr.NewFrame();

	//{
	//	static float f = 0.0f;
	//	ImGui::Text("Hello, world!");
	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	//	ImGui::ColorEdit3("clear color", (float*)&clear_col);
	//	if (ImGui::Button("Test Window")) show_test_window ^= 1;
	//	if (ImGui::Button("Another Window")) show_another_window ^= 1;
	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//}

	//static bool show_test_window = true;
	//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
	//ImGui::ShowTestWindow(&show_test_window);

	//// Render
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
	//	m_renderer.Present();
	//}

	//if (m_isFrustumTracking)
	//	cMainCamera::Get()->PopCamera();
}

//
//void cViewer::OnLostDevice()
//{
//	m_renderer.ResetDevice(0, 0, true);
//	m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
//}
//
//
//void cViewer::OnShutdown()
//{
//}
//

void cViewer::OnEventProc(const sf::Event &evt)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (evt.type)
	{
	case sf::Event::KeyPressed:
		switch (evt.key.code)
		{
		case sf::Keyboard::Escape: close(); break;
		}
		break;
	}
}



//void cViewer::ChangeWindowSize()
//{
//	if (m_renderer.CheckResetDevice())
//	{
//		m_renderer.ResetDevice();
//		m_terrainCamera.SetViewPort(m_renderer.m_viewPort.GetWidth(), m_renderer.m_viewPort.GetHeight());
//	}
//}
//
//void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	framework::cInputManager::Get()->MouseProc(message, wParam, lParam);
//
//	static bool maximizeWnd = false;
//	switch (message)
//	{
//	case WM_EXITSIZEMOVE:
//		ChangeWindowSize();
//		break;
//
//	case WM_SIZE:
//		if (SIZE_MAXIMIZED == wParam)
//		{
//			maximizeWnd = true;
//			ChangeWindowSize();
//		}
//		else if (maximizeWnd && (SIZE_RESTORED == wParam))
//		{
//			maximizeWnd = false;
//			ChangeWindowSize();
//		}
//		break;
//
//	case WM_DROPFILES:
//	{
//		const HDROP hdrop = (HDROP)wParam;
//		char filePath[MAX_PATH];
//		const UINT size = DragQueryFileA(hdrop, 0, filePath, MAX_PATH);
//		if (size == 0)
//			return;// handle error...
//	}
//	break;
//
//	case WM_MOUSEWHEEL:
//	{
//		if (m_isFrustumTracking)
//			cMainCamera::Get()->PushCamera(&m_terrainCamera);
//
//		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
//		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
//		//dbg::Print("%d %d", fwKeys, zDelta);
//
//		const float len = graphic::GetMainCamera().GetDistance();
//		float zoomLen = (len > 100) ? 50 : (len / 4.f);
//		if (fwKeys & 0x4)
//			zoomLen = zoomLen / 10.f;
//
//		graphic::GetMainCamera().Zoom((zDelta<0) ? -zoomLen : zoomLen);
//
//		if (m_isFrustumTracking)
//			cMainCamera::Get()->PopCamera();
//	}
//	break;
//
//	case WM_KEYDOWN:
//		switch (wParam)
//		{
//		case VK_TAB:
//		{
//			static bool flag = false;
//			//m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
//			//m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
//			flag = !flag;
//		}
//		break;
//
//		case VK_RETURN:
//			//cResourceManager::Get()->ReloadShader(m_renderer);
//			break;
//		}
//		break;
//
//	case WM_LBUTTONDOWN:
//	{
//		if (m_isFrustumTracking)
//			cMainCamera::Get()->PushCamera(&m_terrainCamera);
//
//		POINT pos = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
//
//		SetCapture(m_hWnd);
//		m_LButtonDown = true;
//		m_curPos.x = LOWORD(lParam);
//		m_curPos.y = HIWORD(lParam);
//
//		Vector3 orig, dir;
//		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
//		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
//		m_moveLen = common::clamp(1, 100, (p1 - orig).Length());
//		graphic::GetMainCamera().MoveCancel();
//
//		if (m_isFrustumTracking)
//			cMainCamera::Get()->PopCamera();
//	}
//	break;
//
//	case WM_LBUTTONUP:
//		ReleaseCapture();
//		m_LButtonDown = false;
//		break;
//
//	case WM_RBUTTONDOWN:
//	{
//		SetCapture(m_hWnd);
//		m_RButtonDown = true;
//		m_curPos.x = LOWORD(lParam);
//		m_curPos.y = HIWORD(lParam);
//
//		Ray ray(m_curPos.x, m_curPos.y, 1024, 768,
//			GetMainCamera().GetProjectionMatrix(),
//			GetMainCamera().GetViewMatrix());
//	}
//	break;
//
//	case WM_RBUTTONUP:
//		m_RButtonDown = false;
//		ReleaseCapture();
//		break;
//
//	case WM_MBUTTONDOWN:
//		SetCapture(m_hWnd);
//		m_MButtonDown = true;
//		m_curPos.x = LOWORD(lParam);
//		m_curPos.y = HIWORD(lParam);
//		break;
//
//	case WM_MBUTTONUP:
//		ReleaseCapture();
//		m_MButtonDown = false;
//		break;
//
//	case WM_MOUSEMOVE:
//	{
//		if (m_isFrustumTracking)
//			cMainCamera::Get()->PushCamera(&m_terrainCamera);
//
//		sf::Vector2i pos = { (int)LOWORD(lParam), (int)HIWORD(lParam) };
//
//		Vector3 orig, dir;
//		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
//		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
//		//m_line.SetLine(orig + Vector3(1, 0, 0), p1, 0.3f);
//		//m_dbgLine.SetLine(orig + Vector3(-1, 0, 0), p1 + Vector3(-1, 0, 0), 0.3f);
//
//		if (wParam & 0x10) // middle button down
//		{
//		}
//
//		if (m_LButtonDown)
//		{
//			const int x = pos.x - m_curPos.x;
//			const int y = pos.y - m_curPos.y;
//
//			if ((abs(x) > 1000) || (abs(y) > 1000))
//			{
//				if (m_isFrustumTracking)
//					cMainCamera::Get()->PopCamera();
//				break;
//			}
//
//			m_curPos = pos;
//
//			Vector3 dir = graphic::GetMainCamera().GetDirection();
//			Vector3 right = graphic::GetMainCamera().GetRight();
//			dir.y = 0;
//			dir.Normalize();
//			right.y = 0;
//			right.Normalize();
//
//			graphic::GetMainCamera().MoveRight(-x * m_moveLen * 0.001f);
//			graphic::GetMainCamera().MoveFrontHorizontal(y * m_moveLen * 0.001f);
//		}
//		else if (m_RButtonDown)
//		{
//			const int x = pos.x - m_curPos.x;
//			const int y = pos.y - m_curPos.y;
//			m_curPos = pos;
//
//			graphic::GetMainCamera().Yaw2(x * 0.005f);
//			graphic::GetMainCamera().Pitch2(y * 0.005f);
//
//		}
//		else if (m_MButtonDown)
//		{
//			const sf::Vector2i point = { pos.x - m_curPos.x, pos.y - m_curPos.y };
//			m_curPos = pos;
//
//			const float len = graphic::GetMainCamera().GetDistance();
//			graphic::GetMainCamera().MoveRight(-point.x * len * 0.001f);
//			graphic::GetMainCamera().MoveUp(point.y * len * 0.001f);
//		}
//		else
//		{
//		}
//
//		if (m_isFrustumTracking)
//			cMainCamera::Get()->PopCamera();
//	}
//	break;
//	}
//}
//
