//
// DX11 Cascaded ShadowMap
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
	void RenderScene(const char *techniqueName, const bool isShadowMap);


public:
	cCamera m_terrainCamera;
	cGrid m_ground;
	cDbgAxis m_axis;
	cTexture m_texture;
	cModel2 m_model;

	cRenderTarget m_shadowMap;
	cCamera m_lightCamera;
	cBillboard m_dbgShadowQuad;
	cDbgFrustum m_dbgFrustum;
	cDbgFrustum m_frustum[3];

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
	m_windowName = L"DX11 Cascaded ShadowMap";
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
	DragAcceptFiles(m_hWnd, TRUE);
	cAutoCam cam(&m_terrainCamera);

	cResourceManager::Get()->SetMediaDirectory("../media/");

	const float WINSIZE_X = (float)(m_windowRect.right - m_windowRect.left);
	const float WINSIZE_Y = (float)(m_windowRect.bottom - m_windowRect.top);

	m_terrainCamera.Init(&m_renderer);
	m_terrainCamera.SetCamera(Vector3(-10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_terrainCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.0f, 10000.f);
	m_terrainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-3, 3, -3);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_mtrl.InitWhite();

	m_ground.Create(m_renderer, 10, 10, 1, eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE | eVertexType::TEXTURE);
	m_ground.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	m_texture.Create(m_renderer, "../media/whitetex.dds");

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_axis.Create(m_renderer);
	m_axis.SetAxis(bbox2, false);

	m_model.Create(m_renderer, common::GenerateId(), "../media/boxlifter.x");

	m_lightCamera.Init(&m_renderer);
	m_lightCamera.SetCamera(lightPos, lightLookat, Vector3(0, 1, 0));
	m_lightCamera.SetProjectionOrthogonal(5, 5, 0.1f, 10.f);

	cViewport svp = m_renderer.m_viewPort;
	svp.m_vp.MinDepth = 0.f;
	svp.m_vp.MaxDepth = 1.f;
	svp.m_vp.Width = 1024;
	svp.m_vp.Height = 1024;
	m_shadowMap.Create(m_renderer, svp, DXGI_FORMAT_R32_FLOAT);

	m_dbgShadowQuad.Create(m_renderer, BILLBOARD_TYPE::ALL_AXIS, 2, 2, Vector3(0, 0, 3));
	m_dbgFrustum.Create(m_renderer, m_lightCamera);

	for (int i = 0; i < 3; ++i)
		m_frustum[i].Create(m_renderer, m_lightCamera);

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	cAutoCam cam(&m_terrainCamera);
	GetMainCamera().Update(deltaSeconds);
}


void cViewer::OnRender(const float deltaSeconds)
{
	cAutoCam cam(&m_terrainCamera);

	cFrustum::Split3(GetMainCamera(), 0.0003f, 0.001f, 0.003f
		, m_frustum[0], m_frustum[1], m_frustum[2]);
	//for (int i = 0; i < 3; ++i)
	//	m_frustum[i].SetFrustum(m_renderer, m_frustum[i].m_viewProj);

	cShader11 *shader = m_renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE);
	m_texture.Bind(m_renderer, 1);// Unbind ShaderResource

	m_shadowMap.SetRenderTarget(m_renderer);
	if (m_renderer.ClearScene(false, Vector4(1, 1, 1, 1)))
	{
		m_renderer.BeginScene();
		shader->m_shadowMap = NULL;
		RenderScene("BuildShadowMap", true);
		m_renderer.EndScene();
	}
	m_shadowMap.RecoveryRenderTarget(m_renderer);

	cTexture shadowTex(m_shadowMap.m_texture);
	m_dbgShadowQuad.m_texture = &shadowTex;

	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();

		shader->m_shadowMap = m_shadowMap.m_texture;

		RenderScene(m_isShadowRender ? "ShadowMap" : "Unlit", false);
		m_dbgShadowQuad.Render(m_renderer);
		m_ground.Render(m_renderer);
		//m_dbgFrustum.Render(m_renderer);
		for (int i = 0; i < 3; ++i)
			m_frustum[i].Render(m_renderer);

		m_axis.Render(m_renderer);
		m_renderer.EndScene();
		m_renderer.Present();
	}

}


void cViewer::RenderScene(const char *techniqueName, const bool isShadowMap)
{
	if (isShadowMap)
	{
		XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());

		Matrix44 view, proj, tt;
		m_lightCamera.GetShadowMatrix(view, proj, tt);

		m_renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
		m_renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(view.GetMatrixXM());
		m_renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(proj.GetMatrixXM());
	}
	else
	{
		XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetViewMatrix());
		XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetProjectionMatrix());
		XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());

		m_renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(mWorld);
		m_renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
		m_renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);

		Matrix44 view, proj, tt;
		m_lightCamera.GetShadowMatrix(view, proj, tt);
		m_renderer.m_cbPerFrame.m_v->mLightView = XMMatrixTranspose(view.GetMatrixXM());
		m_renderer.m_cbPerFrame.m_v->mLightProj = XMMatrixTranspose(proj.GetMatrixXM());
		m_renderer.m_cbPerFrame.m_v->mLightTT = XMMatrixTranspose(tt.GetMatrixXM());
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
	m_model.Render(m_renderer);
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
		cMainCamera::Get()->PushCamera(&m_terrainCamera);

		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera().GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera().Zoom((zDelta<0) ? -zoomLen : zoomLen);

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
			m_isShadowRender = !m_isShadowRender;
			//cResourceManager::Get()->ReloadShader(m_renderer);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
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
		const bool isPressCtrl = GetAsyncKeyState(VK_LCONTROL);

		cMainCamera::Get()->PushCamera(isPressCtrl ? &m_lightCamera : &m_terrainCamera);

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

		if (isPressCtrl)
			m_dbgFrustum.SetFrustum(m_renderer, m_lightCamera);

		cMainCamera::Get()->PopCamera();
	}
	break;
	}
}

