//
// DX11 Shadowmap
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
	virtual void OnEventProc(const sf::Event &evt) override;


public:
	bool m_isFrustumTracking = true;
	sf::Vector2i m_curPos;
	float m_moveLen;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK3(cViewer);

class cDockView1 : public framework::cDockWindow
{
public:
	cCamera m_terrainCamera;
	cGrid m_ground;
	cDbgArrow m_dbgArrow;
	cDbgAxis m_axis;
	cRenderTarget m_renderTarget;
	cMaterial m_mtrl;
	Transform m_world;

	POINT m_mousePos;
	bool m_mouseDown[3]; // Left, Right, Middle
	POINT m_viewPos;
	sRectf m_viewRect;
	float m_rotateLen;
	Vector3 m_rotateTarget;
	Plane m_groundPlane1;
	Plane m_groundPlane2;

	cDockView1(const string &name) : framework::cDockWindow(name)
		, m_groundPlane1(Vector3(0, 1, 0), 0) {
	}
	virtual ~cDockView1() {
	}

	bool Init() {
		cRenderer &renderer = m_owner->m_renderer;

		const float WINSIZE_X = m_rect.Width();
		const float WINSIZE_Y = m_rect.Height();
		GetMainCamera().Init(&renderer);
		GetMainCamera().SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
		GetMainCamera().SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 10000.0f);
		GetMainCamera().SetViewPort(WINSIZE_X, WINSIZE_Y);

		m_terrainCamera.Init(&renderer);
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

		m_ground.Create(renderer, 10, 10, 1, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE | eVertexType::TEXTURE);
		m_ground.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

		m_mtrl.InitWhite();

		cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
		m_axis.Create(renderer);
		m_axis.SetAxis(bbox2, false);

		m_renderTarget.Create(renderer, (int)WINSIZE_X, (int)WINSIZE_Y);

		return true;
	}

	virtual void OnUpdate(const float deltaSeconds) override {
		cAutoCam cam(&m_terrainCamera);
		GetMainCamera().Update(deltaSeconds);
	}

	virtual void OnPreRender() override {
		cRenderer &renderer = GetRenderer();
		cAutoCam cam(&m_terrainCamera);

		m_renderTarget.SetRenderTarget(renderer);

		if (renderer.ClearScene(false))
		{
			renderer.BeginScene();

			XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetViewMatrix());
			XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetProjectionMatrix());
			XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());
			renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
			renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
			renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);

			m_ground.Render(renderer);
			m_axis.Render(renderer);
			renderer.EndScene();
		}

		m_renderTarget.RecoveryRenderTarget(renderer);
	}

	virtual void OnRender() override {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		m_viewPos = { (int)(pos.x), (int)(pos.y) };
		m_viewRect = { pos.x + 5, pos.y, pos.x + m_rect.Width() - 30, pos.y + m_rect.Height() - 70 };
		ImGui::Image(m_renderTarget.m_texture, ImVec2(m_rect.Width(), m_rect.Height() - 70));
	}

	virtual void OnResizeEnd(const framework::eDockResize::Enum type, const sRectf &rect) override {
		if (type == eDockResize::DOCK_WINDOW)
		{
			m_owner->RequestResetDeviceNextFrame();
		}
	}
	
	void UpdateLookAt()
	{
		//cAutoCam cam(&g_root.m_camWorld);

		GetMainCamera().MoveCancel();

		const float centerX = GetMainCamera().m_width / 2;
		const float centerY = GetMainCamera().m_height / 2;
		Vector3 orig, dir;
		GetMainCamera().GetRay((int)centerX, (int)centerY, orig, dir);
		const float distance = m_groundPlane1.Collision(dir);
		if (distance < -0.2f)
		{
			GetMainCamera().m_lookAt = m_groundPlane1.Pick(orig, dir);
		}
		else
		{ // horizontal viewing
			const Vector3 lookAt = GetMainCamera().m_eyePos + GetMainCamera().GetDirection() * 50.f;
			GetMainCamera().m_lookAt = lookAt;
		}

		GetMainCamera().UpdateViewMatrix();
	}


	// 휠을 움직였을 때,
	// 카메라 앞에 박스가 있다면, 박스 정면에서 멈춘다.
	void OnWheelMove(const int delta, const POINT mousePt)
	{
		UpdateLookAt();

		float len = 0;
		{
			Vector3 orig, dir;
			GetMainCamera().GetRay(orig, dir);
			Vector3 lookAt = m_groundPlane1.Pick(orig, dir);
			len = (orig - lookAt).Length();
		}

		// zoom in/out
		float zoomLen = 0;
		if (len > 100)
			zoomLen = 50;
		else if (len > 50)
			zoomLen = max(1.f, (len / 2.f));
		else
			zoomLen = (len / 3.f);

		Vector3 dir = GetMainCamera().GetDirection();
		Vector3 eyePos = GetMainCamera().m_eyePos + dir * ((delta <= 0) ? -zoomLen : zoomLen);
		if (eyePos.y > 1)
			GetMainCamera().Zoom(dir, (delta < 0) ? -zoomLen : zoomLen);
	}


	// Handling Mouse Move Event
	void OnMouseMove(const POINT mousePt)
	{
		const POINT delta = { mousePt.x - m_mousePos.x, mousePt.y - m_mousePos.y };
		m_mousePos = mousePt;

		if (m_mouseDown[0])
		{
			Vector3 dir = GetMainCamera().GetDirection();
			Vector3 right = GetMainCamera().GetRight();
			dir.y = 0;
			dir.Normalize();
			right.y = 0;
			right.Normalize();

			GetMainCamera().MoveRight(-delta.x * m_rotateLen * 0.001f);
			GetMainCamera().MoveFrontHorizontal(delta.y * m_rotateLen * 0.001f);
		}
		else if (m_mouseDown[1])
		{
			GetMainCamera().Yaw4(delta.x * 0.005f, Vector3(0, 1, 0), m_rotateTarget);
			GetMainCamera().Pitch4(delta.y * 0.005f, Vector3(0, 1, 0), m_rotateTarget);

			if (GetMainCamera().m_eyePos.y < 1)
			{
				GetMainCamera().m_eyePos.y = 1;
				GetMainCamera().UpdateViewMatrix();
			}
		}
		else if (m_mouseDown[2])
		{
			const float len = GetMainCamera().GetDistance();
			GetMainCamera().MoveRight(-delta.x * len * 0.001f);
			GetMainCamera().MoveUp(delta.y * len * 0.001f);
		}
		else
		{
			cInputManager::Get()->m_mousePt = mousePt;
		}
	}


	// Handling Mouse Button Down Event
	void OnMouseDown(const sf::Mouse::Button &button, const POINT mousePt)
	{
		m_mousePos = mousePt;
		SetCapture();

		switch (button)
		{
		case sf::Mouse::Left:
		{
			m_mouseDown[0] = true;

			Vector3 orig, dir;
			GetMainCamera().GetRay(mousePt.x, mousePt.y, orig, dir);
			Vector3 p1 = m_groundPlane1.Pick(orig, dir);
			m_rotateLen = min(500.f, (p1 - orig).Length());
		}
		break;

		case sf::Mouse::Right:
			m_mouseDown[1] = true;
			break;

		case sf::Mouse::Middle:
		{
			m_mouseDown[2] = true;
			UpdateLookAt();

			Vector3 orig, dir;
			GetMainCamera().GetRay(mousePt.x, mousePt.y, orig, dir);
			Vector3 target = m_groundPlane1.Pick(orig, dir);

			const float len = (GetMainCamera().GetEyePos() - target).Length();
			m_rotateTarget = target;
		}
		break;
		}
	}

	void OnMouseUp(const sf::Mouse::Button &button, const POINT mousePt)
	{
		const POINT delta = { mousePt.x - m_mousePos.x, mousePt.y - m_mousePos.y };
		m_mousePos = mousePt;
		ReleaseCapture();

		switch (button)
		{
		case sf::Mouse::Left:
			ReleaseCapture();
			m_mouseDown[0] = false;
			break;
		case sf::Mouse::Right:
			m_mouseDown[1] = false;
			ReleaseCapture();
			break;
		case sf::Mouse::Middle:
			m_mouseDown[2] = false;
			ReleaseCapture();
			break;
		}
	}

	void OnEventProc(const sf::Event &evt)
	{
		cInputManager::Get()->MouseProc(evt);

		ImGuiIO& io = ImGui::GetIO();
		switch (evt.type)
		{
		case sf::Event::KeyPressed:
			switch (evt.key.code)
			{
			case sf::Keyboard::Return:
			{
			}
			break;

			case sf::Keyboard::Space:
			{
				//DirectX::SaveWICTextureToFile(GetRenderer().GetDevContext()
				//	, m_renderTarget.m_rawTex, GUID_ContainerFormatPng, L"ss.png");
			}
			break;

			//case sf::Keyboard::Left: g_root.m_camWorld.MoveRight(-0.5f); break;
			//case sf::Keyboard::Right: g_root.m_camWorld.MoveRight(0.5f); break;
			//case sf::Keyboard::Up: g_root.m_camWorld.MoveUp(0.5f); break;
			//case sf::Keyboard::Down: g_root.m_camWorld.MoveUp(-0.5f); break;
			}
			break;

		case sf::Event::MouseMoved:
		{
			cAutoCam cam(&m_terrainCamera);

			POINT curPos;
			GetCursorPos(&curPos); // sf::event mouse position has noise so we use GetCursorPos() function
			ScreenToClient(m_owner->getSystemHandle(), &curPos);
			POINT pos = { curPos.x - m_viewPos.x, curPos.y - m_viewPos.y };
			OnMouseMove(pos);
		}
		break;

		case sf::Event::MouseButtonPressed:
		case sf::Event::MouseButtonReleased:
		{
			cAutoCam cam(&m_terrainCamera);

			POINT curPos;
			GetCursorPos(&curPos); // sf::event mouse position has noise so we use GetCursorPos() function
			ScreenToClient(m_owner->getSystemHandle(), &curPos);
			POINT pos = { curPos.x - m_viewPos.x, curPos.y - m_viewPos.y };
			//if (m_viewRect.IsIn((float)curPos.x, (float)curPos.y))
			{
				if (sf::Event::MouseButtonPressed == evt.type)
					OnMouseDown(evt.mouseButton.button, pos);
				else
					OnMouseUp(evt.mouseButton.button, pos);
			}
		}
		break;

		case sf::Event::MouseWheelMoved:
		{
			cAutoCam cam(&m_terrainCamera);

			POINT curPos;
			GetCursorPos(&curPos); // sf::event mouse position has noise so we use GetCursorPos() function
			ScreenToClient(m_owner->getSystemHandle(), &curPos);
			const POINT pos = { curPos.x - m_viewPos.x, curPos.y - m_viewPos.y };
			OnWheelMove(evt.mouseWheel.delta, pos);
		}
		break;
		}
	}

	virtual void OnLostDevice() {
	}

	virtual void OnResetDevice() {
		m_terrainCamera.SetViewPort(m_rect.Width(), m_rect.Height());
		m_renderTarget.Create(GetRenderer(), (int)m_rect.Width(), (int)m_rect.Height());
	}

};


cViewer::cViewer()
{
	m_windowName = L"DX11 Shadowmap";
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
}


bool cViewer::OnInit()
{
	DragAcceptFiles(m_hWnd, TRUE);

	cResourceManager::Get()->SetMediaDirectory("../media/");

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


	const int cx = GetSystemMetrics(SM_CXSCREEN);
	const int cy = GetSystemMetrics(SM_CYSCREEN);

	m_gui.SetContext();

	float col_main_hue = 0.0f / 255.0f;
	float col_main_sat = 0.0f / 255.0f;
	float col_main_val = 80.0f / 255.0f;

	float col_area_hue = 0.0f / 255.0f;
	float col_area_sat = 0.0f / 255.0f;
	float col_area_val = 50.0f / 255.0f;

	float col_back_hue = 0.0f / 255.0f;
	float col_back_sat = 0.0f / 255.0f;
	float col_back_val = 35.0f / 255.0f;

	float col_text_hue = 0.0f / 255.0f;
	float col_text_sat = 0.0f / 255.0f;
	float col_text_val = 255.0f / 255.0f;
	float frameRounding = 0.0f;

	ImVec4 col_text = ImColor::HSV(col_text_hue, col_text_sat, col_text_val);
	ImVec4 col_main = ImColor::HSV(col_main_hue, col_main_sat, col_main_val);
	ImVec4 col_area = ImColor::HSV(col_area_hue, col_area_sat, col_area_val);
	ImVec4 col_back = ImColor::HSV(col_back_hue, col_back_sat, col_back_val);
	float rounding = frameRounding;

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = rounding;
	style.WindowRounding = rounding;
	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.80f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.54f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.10f, 0.10f, 0.10f, 0.55f);

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	__super::OnUpdate(deltaSeconds);
}

void cViewer::OnRender(const float deltaSeconds)
{
	__super::OnRender(deltaSeconds);
}

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
