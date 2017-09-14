//
// DX11 Cascaded PCF ShadowMap
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
	void RenderScene(const char *techniqueName, const bool isShadowMap, const cCamera lightCam[3]);


public:
	cCamera m_mainCamera;
	cCamera m_secondCamera;
	cGrid m_ground;
	cDbgAxis m_axis;
	cTexture m_texture;
	cModel2 m_model;

	cCamera m_lightCamera;
	cDbgFrustum m_dbgLightFrustum;
	cDbgFrustum m_dbgMainFrustum;

	// cascaded shadow map
	cDbgFrustum m_frustum[3];
	cCamera m_lightSplitCamera[3];
	cDbgQuad m_cascadedGround[3];
	cRenderTarget m_shadowMap[3];
	cQuad2D m_shadowMapQuad[3];
	cDbgQuad m_testQuad;

	cText m_text[6];

	bool m_showSecondCam = false;
	bool m_showMainFrustum = false;
	bool m_showLightFrustum1 = false;
	bool m_showLightFrustum2 = false;
	bool m_showCascade = false;
	bool m_showShadowMap = false;

	Transform m_world;
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
{
	m_windowName = L"DX11 PCF ShadowMap";
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

	m_mainCamera.Init(&m_renderer);
	m_mainCamera.SetCamera(Vector3(-10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_mainCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.0f, 10000.f);
	m_mainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	m_secondCamera.Init(&m_renderer);
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

	m_ground.Create(m_renderer, 10, 10, 100, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE
		, cColor::WHITE, g_defaultTexture);
	//m_ground.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	m_texture.Create(m_renderer, "../media/whitetex.dds");

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_axis.Create(m_renderer);
	m_axis.SetAxis(bbox2, false);

	m_model.Create(m_renderer, common::GenerateId(), "../media/boxlifter.x");

	m_lightCamera.Init(&m_renderer);
	m_lightCamera.SetCamera(lightPos, lightLookat, Vector3(0, 1, 0));
	m_lightCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.0f, 10000.f);

	cViewport svp = m_renderer.m_viewPort;
	svp.m_vp.MinDepth = 0.f;
	svp.m_vp.MaxDepth = 1.f;
	svp.m_vp.Width = 1024;
	svp.m_vp.Height = 1024;
	//m_shadowMap.Create(m_renderer, svp, DXGI_FORMAT_R32_FLOAT);
	m_dbgLightFrustum.Create(m_renderer, m_lightCamera);
	m_dbgLightFrustum.SetFrustum(m_renderer, m_dbgLightFrustum.m_viewProj);

	m_dbgMainFrustum.Create(m_renderer, m_mainCamera);
	m_dbgMainFrustum.SetFrustum(m_renderer, m_dbgMainFrustum.m_viewProj);

	const Vector3 billboardPos[3] = { Vector3(0,0,3), Vector3(0,0,7), Vector3(0,0,11) };
	for (int i = 0; i < 3; ++i)
	{
		m_shadowMapQuad[i].Create(m_renderer, 0, 310 * i, 300, 300);

		m_frustum[i].Create(m_renderer, m_lightCamera);
		m_cascadedGround[i].Create(m_renderer);
		m_shadowMap[i].Create(m_renderer, svp, DXGI_FORMAT_R32_FLOAT);
	}

	for (int i = 0; i < 6; ++i)
		m_text[i].Create(m_renderer, 18, false, "Arial", cColor::BLACK);

	const Vector3 quadVtx[] = {
		Vector3(-5,0,5)
		, Vector3(5,0,5)
		, Vector3(5,0,-5)
		, Vector3(-5,0,-5)
	};
	m_testQuad.Create(m_renderer);
	m_testQuad.SetQuad(quadVtx, 0.05f);

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	cAutoCam cam(&m_mainCamera);
	GetMainCamera().Update(deltaSeconds);

	{
		cFrustum::Split3(GetMainCamera(), 0.0003f, 0.001f, 0.003f
			, m_frustum[0], m_frustum[1], m_frustum[2]);
		for (int i = 0; i < 3; ++i)
		{
			Vector3 vtxQuad[4];
			const Plane ground(Vector3(0, 1, 0), 0);
			m_frustum[i].GetGroundPlaneVertices(ground, vtxQuad);

			m_lightSplitCamera[i] = m_lightCamera;
			//m_lightSplitCamera[i].FitQuad(vtxQuad);
			m_lightSplitCamera[i].FitFrustum(m_frustum[i].m_viewProj);
			m_frustum[i].SetFrustum(m_renderer, m_lightSplitCamera[i]);
			m_cascadedGround[i].SetQuad(vtxQuad, 0.05f);
		}
	}
}


void cViewer::OnRender(const float deltaSeconds)
{
	cAutoCam cam(m_showSecondCam ? &m_secondCamera : &m_mainCamera);

	cShader11 *shader = m_renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE);

	CommonStates states(m_renderer.GetDevice());
	m_renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	m_renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
	m_renderer.GetDevContext()->OMSetBlendState(states.Opaque(), 0, 0xffffffff);

	m_texture.Unbind(m_renderer, 1);
	m_texture.Unbind(m_renderer, 2);
	m_texture.Unbind(m_renderer, 3);

	for (int i = 0; i < 3; ++i)
	{
		cAutoCam lightCam(&m_lightSplitCamera[i]);
		m_shadowMap[i].SetRenderTarget(m_renderer);
		if (m_renderer.ClearScene(false, Vector4(1, 1, 1, 1)))
		{
			m_renderer.BeginScene();
			ZeroMemory(shader->m_textureMap, sizeof(shader->m_textureMap));
			RenderScene("BuildShadowMap", true, m_lightSplitCamera);
			m_renderer.EndScene();
		}
		m_shadowMap[i].RecoveryRenderTarget(m_renderer);
	}


	m_renderer.GetDevContext()->OMSetBlendState(states.AlphaBlend(), 0, 0xffffffff);
	cTexture shadowTex0(m_shadowMap[0].m_texture);
	cTexture shadowTex1(m_shadowMap[1].m_texture);
	cTexture shadowTex2(m_shadowMap[2].m_texture);
	m_shadowMapQuad[0].m_texture = &shadowTex0;
	m_shadowMapQuad[1].m_texture = &shadowTex1;
	m_shadowMapQuad[2].m_texture = &shadowTex2;

	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();

		shader->SetBindTexture(&shadowTex0, 1);
		shader->SetBindTexture(&shadowTex1, 2);
		shader->SetBindTexture(&shadowTex2, 3);

		RenderScene(m_isShadowRender ? "ShadowMap" : "Unlit", false, m_lightSplitCamera);

		if (m_showMainFrustum)
			m_dbgMainFrustum.Render(m_renderer);
		if (m_showLightFrustum1)
			m_dbgLightFrustum.Render(m_renderer);

		for (int i = 0; i < 3; ++i)
		{
			if (m_showLightFrustum2)
				m_frustum[i].Render(m_renderer);
			if (m_showCascade)
				m_cascadedGround[i].Render(m_renderer);
		}

		m_axis.Render(m_renderer);

		//m_testQuad.Render(m_renderer);

		if (m_showShadowMap)
			for (int i = 0; i < 3; ++i)
				m_shadowMapQuad[i].Render(m_renderer);

		static WStr64 str[6];
		str[0].Format(L"1: Show Second Camera = %d", m_showSecondCam);
		str[1].Format(L"2: Show Main Frustum = %d", m_showMainFrustum);
		str[2].Format(L"3: Show Light Frustum1 = %d", m_showLightFrustum1);
		str[3].Format(L"4: Show Light Frustum2 = %d", m_showLightFrustum2);
		str[4].Format(L"5: Show Cascade = %d", m_showCascade);
		str[5].Format(L"6: Show ShadowMap = %d", m_showShadowMap);

		for (int i = 0; i < 6; ++i)
		{
			m_text[i].SetText(str[i].c_str());
			m_text[i].Render(m_renderer, 150, 50 + 20 * i);
		}

		m_renderer.RenderFPS();

		m_renderer.EndScene();
		m_renderer.Present();
	}
}


void cViewer::RenderScene(const char *techniqueName, const bool isShadowMap
	, const cCamera lightCam[3])
{
	if (isShadowMap)
	{
		XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&GetMainCamera().GetViewMatrix());
		XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&GetMainCamera().GetProjectionMatrix());
		XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());

		m_renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
		m_renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
		m_renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);
	}
	else
	{
		XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&GetMainCamera().GetViewMatrix());
		XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&GetMainCamera().GetProjectionMatrix());
		XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());

		m_renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
		m_renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
		m_renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);

		for (int i = 0; i < 3; ++i)
		{
			Matrix44 view, proj, tt;
			lightCam[i].GetShadowMatrix(view, proj, tt);
			m_renderer.m_cbPerFrame.m_v->mLightView[i] = XMMatrixTranspose(view.GetMatrixXM());
			m_renderer.m_cbPerFrame.m_v->mLightProj[i] = XMMatrixTranspose(proj.GetMatrixXM());
			m_renderer.m_cbPerFrame.m_v->mLightTT = XMMatrixTranspose(tt.GetMatrixXM());
		}
	}

	const Vector3 lightPos = m_lightCamera.GetEyePos();
	const Vector3 lightLookat = m_lightCamera.GetLookAt();
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());
	m_renderer.m_cbLight = GetMainLight().GetLight();
	m_renderer.m_cbLight.Update(m_renderer, 1);
	m_renderer.m_cbMaterial = m_mtrl.GetMaterial();
	m_renderer.m_cbMaterial.Update(m_renderer, 2);

	m_model.m_techniqueName = techniqueName;

	for (int k = 0; k < 10; ++k)
	{
		XMMATRIX tms[100];
		for (int x = 0; x < 10; ++x)
		{
			for (int y = 0; y < 10; ++y)
			{
				Transform tfm;
				tfm.pos = Vector3(x*3.f + k*30, 0, y*3.f);
				tms[y * 10 + x] = tfm.GetMatrixXM();
			}
		}
		m_model.RenderInstancing(m_renderer, 100, tms);
	}

	// White Material
	m_renderer.m_cbMaterial = m_mtrl.GetMaterial();
	m_renderer.m_cbMaterial.Update(m_renderer, 2);
	m_ground.m_techniqueName = techniqueName;
	m_ground.Render(m_renderer);
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

		Vector3 orig, dir;
		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
		Vector3 p1 = m_groundPlane1.Pick(orig, dir);
		m_moveLen = common::clamp(1, 100, (p1 - orig).Length());
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
		const bool isPressCtrl = GetAsyncKeyState(VK_LCONTROL);

		cCamera *camera = m_showSecondCam ? &m_secondCamera : &m_mainCamera;
		cAutoCam cam(isPressCtrl ? &m_lightCamera : camera);

		sf::Vector2i pos = { (int)LOWORD(lParam), (int)HIWORD(lParam) };

		Vector3 orig, dir;
		graphic::GetMainCamera().GetRay(pos.x, pos.y, orig, dir);
		Vector3 p1 = m_groundPlane1.Pick(orig, dir);

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

		if (isPressCtrl)
			m_dbgLightFrustum.SetFrustum(m_renderer, m_lightCamera);

		m_dbgMainFrustum.SetFrustum(m_renderer, m_mainCamera);
	}
	break;
	}
}

