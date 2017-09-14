#pragma once

#include "dx11normalmap.h"


using namespace graphic;
using namespace framework;


class cDockView1 : public framework::cDockWindow
{
public:
	graphic::cCamera m_terrainCamera;
	graphic::cRenderTarget m_renderTarget;
	Transform m_world;

	bool m_isLightRender;
	POINT m_mousePos;
	POINT m_viewPos;
	sRectf m_viewRect;
	float m_rotateLen;
	Vector3 m_rotateTarget;
	Plane m_groundPlane1;
	Plane m_groundPlane2;

	cDockView1(const string &name) : framework::cDockWindow(name)
		, m_groundPlane1(Vector3(0, 1, 0), 0)
		, m_isLightRender(false) {
	}
	virtual ~cDockView1() {
	}

	bool Init() {
		cRenderer &renderer = m_owner->m_renderer;

		const float WINSIZE_X = m_rect.Width();
		const float WINSIZE_Y = m_rect.Height();

		m_terrainCamera.Init(&renderer);
		m_terrainCamera.SetCamera(Vector3(-3, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
		m_terrainCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.0f, 10000.f);
		m_terrainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

		cAutoCam cam(m_isLightRender ? &g_lightCamera : &m_terrainCamera);

		cViewport vp = renderer.m_viewPort;
		vp.m_vp.Width = WINSIZE_X;
		vp.m_vp.Height = WINSIZE_Y;
		m_renderTarget.Create(renderer, vp);

		return true;
	}

	virtual void OnUpdate(const float deltaSeconds) override {
		GetMainCamera().Update(deltaSeconds);
	}

	virtual void OnPreRender() override {
		if (m_isLightRender)
			return;

		cRenderer &renderer = GetRenderer();
		cAutoCam cam(&m_terrainCamera);

		g_unBindTex.Bind(renderer, 1);// Unbind ShaderResource

		m_renderTarget.SetRenderTarget(renderer);
		//renderer.ClearScene(false);
		if (renderer.ClearScene(false))
		{
			renderer.BeginScene();

			CommonStates states(renderer.GetDevice());
			renderer.GetDevContext()->OMSetBlendState(states.AlphaBlend(), 0, 0xffffffff);

			GetMainCamera().Bind(renderer);
			GetMainLight().Set(g_lightCamera);
			GetMainLight().Bind(renderer);

			Matrix44 view, proj, tt;
			g_lightCamera.GetShadowMatrix(view, proj, tt);
			renderer.m_cbPerFrame.m_v->mLightView[0] = XMMatrixTranspose(view.GetMatrixXM());
			renderer.m_cbPerFrame.m_v->mLightProj[0] = XMMatrixTranspose(proj.GetMatrixXM());
			renderer.m_cbPerFrame.m_v->mLightTT = XMMatrixTranspose(tt.GetMatrixXM());

			cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE);

			renderer.UnbindTextureAll();
			renderer.BindTexture(g_shadowMap, 2);

			g_viewer->m_ground.m_techniqueName = "ShadowMap";
			g_viewer->m_model2.m_techniqueName = "ShadowMap";

			g_viewer->m_model2.Render(renderer);
			g_viewer->m_ground.Render(renderer);
			g_viewer->m_axis.Render(renderer);


			renderer.GetDevContext()->OMSetBlendState(states.Opaque(), 0, 0xffffffff);
			renderer.EndScene();
		}

		m_renderTarget.RecoveryRenderTarget(renderer);
	}

	virtual void OnRender() override {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		m_viewPos = { (int)(pos.x), (int)(pos.y) };
		m_viewRect = { pos.x + 5, pos.y, pos.x + m_rect.Width() - 30, pos.y + m_rect.Height() - 70 };
		ImGui::Image(m_isLightRender ? g_shadowMap.m_texture : m_renderTarget.m_texture
			, ImVec2(m_rect.Width(), m_rect.Height() - 70));
	}

	virtual void OnResizeEnd(const framework::eDockResize::Enum type, const sRectf &rect) override {
		if (type == eDockResize::DOCK_WINDOW)
		{
			m_owner->RequestResetDeviceNextFrame();
		}
	}


	void UpdateLookAt()
	{
		cAutoCam cam(m_isLightRender ? &g_lightCamera : &m_terrainCamera);

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
		if (m_isLightRender)
			return;

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
		ImGuiIO &io = ImGui::GetIO();

		if (io.MouseDown[0])
		{
			Vector3 dir = GetMainCamera().GetDirection();
			Vector3 right = GetMainCamera().GetRight();
			dir.y = 0;
			dir.Normalize();
			right.y = 0;
			right.Normalize();

			if (!m_isLightRender)
			{
				GetMainCamera().MoveRight(-delta.x * m_rotateLen * 0.001f);
				GetMainCamera().MoveFrontHorizontal(delta.y * m_rotateLen * 0.001f);
			}
		}
		else if (io.MouseDown[1])
		{
			GetMainCamera().Yaw4(delta.x * 0.005f, Vector3(0, 1, 0), m_rotateTarget);
			GetMainCamera().Pitch4(delta.y * 0.005f, Vector3(0, 1, 0), m_rotateTarget);

			if (GetMainCamera().m_eyePos.y < 1)
			{
				GetMainCamera().m_eyePos.y = 1;
				GetMainCamera().UpdateViewMatrix();
			}
		}
		else if (io.MouseDown[2])
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
			Vector3 orig, dir;
			GetMainCamera().GetRay(mousePt.x, mousePt.y, orig, dir);
			Vector3 p1 = m_groundPlane1.Pick(orig, dir);
			m_rotateLen = min(500.f, (p1 - orig).Length());
		}
		break;

		case sf::Mouse::Right:
			break;

		case sf::Mouse::Middle:
		{
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
			break;
		case sf::Mouse::Right:
			ReleaseCapture();
			break;
		case sf::Mouse::Middle:
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
				DirectX::SaveWICTextureToFile(GetRenderer().GetDevContext()
					, g_shadowMap.m_rawTex, GUID_ContainerFormatPng, L"ss.png");
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
			cAutoCam cam(m_isLightRender ? &g_lightCamera : &m_terrainCamera);

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
			dbg::Log("MouseButton Press/Release \n");

			cAutoCam cam(m_isLightRender ? &g_lightCamera : &m_terrainCamera);

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
			cAutoCam cam(m_isLightRender ? &g_lightCamera : &m_terrainCamera);

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
		dbg::Log("%s ResetDevoce\n", m_name.c_str());

		//cAutoCam cam(m_isLightRender ? &g_lightCamera : &m_terrainCamera);
		if (m_isLightRender)
		{
			//g_lightCamera.SetViewPort(m_rect.Width(), m_rect.Height());
		}
		else
		{
			m_terrainCamera.SetViewPort(m_rect.Width(), m_rect.Height());
		}

		cViewport vp = GetRenderer().m_viewPort;
		vp.m_vp.Width = m_rect.Width();
		vp.m_vp.Height = m_rect.Height();
		m_renderTarget.Create(GetRenderer(), vp);
	}

};
