//
// DX11 Primitive
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
	cGrid m_ground;
	cCube m_cube;
	cShader11 m_gridShader;
	cShader11 m_cubeShader;
	cShader11 m_dbgShader;
	cTexture m_texture;
	cDbgBox m_dbgBox;

	cConstantBuffer m_cbPerFrame;
	cConstantBuffer m_cbLight;
	cConstantBuffer m_cbMaterial;

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

INIT_FRAMEWORK(cViewer);


struct sCbPerFrame
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMVECTOR eyePosW;
};



cViewer::cViewer()
	: m_groundPlane1(Vector3(0, 1, 0), 0)
{
	m_windowName = L"DX11 Primitive";
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

	//cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = m_windowRect.right - m_windowRect.left;
	const int WINSIZE_Y = m_windowRect.bottom - m_windowRect.top;
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 10000.0f);
	GetMainCamera()->SetViewPort(WINSIZE_X, WINSIZE_Y);

	m_terrainCamera.Init(&m_renderer);
	m_terrainCamera.SetCamera(Vector3(10, 10, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_terrainCamera.SetProjection(MATH_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.0f, 10000.f);
	m_terrainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_gridShader.Create(m_renderer, "../media/shader11/grid.fxo", "LightTech", layout, ARRAYSIZE(layout));


	D3D11_INPUT_ELEMENT_DESC cubeLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_cubeShader.Create(m_renderer, "../media/shader11/cubetex-light.fxo", "LightTech", cubeLayout, ARRAYSIZE(cubeLayout));

	D3D11_INPUT_ELEMENT_DESC dbgLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_dbgShader.Create(m_renderer, "../media/shader11/dbg.fxo", "LightTech", dbgLayout, ARRAYSIZE(dbgLayout));


	// Create the constant buffer
	m_cbPerFrame.Create(m_renderer, sizeof(sCbPerFrame));
	m_cbLight.Create(m_renderer, sizeof(sCbLight));
	m_cbMaterial.Create(m_renderer, sizeof(sCbMaterial));

	m_ground.Create(m_renderer, 10, 10, 1, eGridType::POSITION | eGridType::NORMAL | eGridType::DIFFUSE | eGridType::TEXTURE);

	cBoundingBox bbox;
	bbox.SetBoundingBox(Vector3(0,0,0), Vector3(2, 2, 2));
	m_cube.Create(m_renderer, bbox, eCubeType::POSITION | eCubeType::NORMAL | eCubeType::DIFFUSE | eGridType::TEXTURE);

	m_texture.Create(m_renderer, "../media/BoxEdgebg_Wood_black.dds");
	m_cube.m_texture = &m_texture;
	m_ground.m_texture = &m_texture;

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 300, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_mtrl.InitWhite();

	m_dbgBox.Create(m_renderer);
	m_dbgBox.SetBox(m_cube.m_boundingBox);

	//m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	//m_renderer.GetDevice()->LightEnable(0, true);

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	GetMainCamera()->Update(deltaSeconds);
}

bool show_test_window = true;
bool show_another_window = false;
//ImVec4 clear_col = ImColor(114, 144, 154);

void cViewer::OnRender(const float deltaSeconds)
{
	if (m_isFrustumTracking)
		cMainCamera::Get()->PushCamera(&m_terrainCamera);

	//GetMainLight().Bind(m_renderer, 0);

	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();

		GetMainCamera()->Bind(m_renderer);

		static float t = 0;
		t += deltaSeconds;
		//m_world.rot.SetRotationY(t);

		XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetViewMatrix());
		XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_terrainCamera.GetProjectionMatrix());
		XMMATRIX mWorld = XMLoadFloat4x4((XMFLOAT4X4*)&m_world.GetMatrix());
		sCbPerFrame cb0;
		cb0.mWorld = XMMatrixTranspose(mWorld);
		cb0.mView = XMMatrixTranspose(mView);
		cb0.mProjection = XMMatrixTranspose(mProj);
		m_cbPerFrame.Update(m_renderer, &cb0);

		sCbLight cb1 = GetMainLight().GetLight();
		m_cbLight.Update(m_renderer, &cb1);

		sCbMaterial cb2 = m_mtrl.GetMaterial();
		m_cbMaterial.Update(m_renderer, &cb2);

		const int pass = m_gridShader.Begin();
		for (int i = 0; i < pass; ++i)
		{
			m_gridShader.BeginPass(m_renderer, i);
			m_cbPerFrame.Bind(m_renderer);
			m_cbLight.Bind(m_renderer, 1);
			m_cbMaterial.Bind(m_renderer, 2);
			m_ground.Render(m_renderer);
		}


		cb0.mWorld = XMMatrixTranspose(m_cube.m_boundingBox.GetTransform());
		m_cbPerFrame.Update(m_renderer, &cb0);
		const int pass2 = m_cubeShader.Begin();
		for (int i = 0; i < pass2; ++i)
		{
			m_cubeShader.BeginPass(m_renderer, i);
			m_cbPerFrame.Bind(m_renderer);
			m_cbLight.Bind(m_renderer, 1);
			m_cbMaterial.Bind(m_renderer, 2);
			m_cube.Render(m_renderer);
		}


		cb0.mWorld = XMMatrixTranspose(m_dbgBox.m_boundingBox.GetTransform());
		m_cbPerFrame.Update(m_renderer, &cb0);
		const int pass3 = m_dbgShader.Begin();
		for (int i = 0; i < pass3; ++i)
		{
			m_dbgShader.BeginPass(m_renderer, i);
			m_cbPerFrame.Bind(m_renderer);
			m_dbgBox.Render(m_renderer);
		}


		m_renderer.EndScene();
		m_renderer.Present();
	}

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
		//m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
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
			//m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			//m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case VK_RETURN:
			//cResourceManager::Get()->ReloadShader(m_renderer);
			break;

			//case VK_SPACE: m_isShadow = !m_isShadow; break;
			//case '1': m_isShowLightFrustum = !m_isShowLightFrustum; break;
			//case '2':
			//{
			//	// Switching Camera Option
			//	if (m_isFrustumTracking)
			//	{
			//		GetMainCamera()->SetEyePos(m_terrainCamera.GetEyePos());
			//		GetMainCamera()->SetLookAt(m_terrainCamera.GetLookAt());
			//	}
			//	else
			//	{
			//		m_terrainCamera.SetEyePos(GetMainCamera()->GetEyePos());
			//		m_terrainCamera.SetLookAt(GetMainCamera()->GetLookAt());
			//	}
			//	m_isFrustumTracking = !m_isFrustumTracking;
			//}
			//break;

			//case '3': m_isCullingModel = !m_isCullingModel;  break;
			//case '4': {
			//	static bool isDbgRender = false;
			//	isDbgRender = !isDbgRender;
			//}
			//		  break;
			//case '5': m_isShowFrustum = !m_isShowFrustum; break;
			//case '6': m_isShowFrustumQuad = !m_isShowFrustumQuad; break;
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

