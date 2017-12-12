//
// DX11 Tessellation Quad Tree
//
#include "../../../../../Common/Common/common.h"
using namespace common;
#include "../../../../../Common/Graphic11/graphic11.h"
#include "../../../../../Common/Framework11/framework11.h"

using namespace graphic;

struct sMatrixBuffer
{
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};

struct sTessellationBuffer
{
	Vector4 eyePos;
	float tessellationAmount;
	Vector3 padding;
};

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
	cGridLine m_ground;
	cDbgAxis m_axis;
	cVertexBuffer m_vtxBuffer;
	bool m_isWireframe = true;

	cShader11 m_shader;
	cConstantBuffer<sMatrixBuffer> m_cbMatrix;
	cConstantBuffer<sTessellationBuffer> m_cbTessellation;

	cTexture m_texture;
	float m_tesellationAmount = 1; // Press Up/Down Button to Change

	cMaterial m_mtrl;

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
	, m_mainCamera("viewer camera")
{
	m_windowName = L"DX11 Tessellation Quad-Tree";
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

	m_mainCamera.SetCamera(Vector3(-10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	m_mainCamera.SetProjection(MATH_PI / 4.f, WINSIZE_X / WINSIZE_Y, 1.0f, 10000.f);
	m_mainCamera.SetViewPort(WINSIZE_X, WINSIZE_Y);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 400, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_mtrl.InitWhite();

	m_ground.Create(m_renderer, 100, 100, 10, 10);

	cBoundingBox bbox2(Vector3(0, 0, 0), Vector3(10, 10, 10), Quaternion());
	m_axis.Create(m_renderer);
	m_axis.SetAxis(bbox2, false);

	struct sVertexType {
		Vector3 p;
		Vector2 t0;
	};

	const float size = 10.f;
	vector<sVertexType> vertices;
	vertices.reserve(100 * 100);
	for (int i = 0; i < 100; ++i)
	{
		for (int k = 0; k < 100; ++k)
		{
			vertices.push_back(
			{ Vector3((float)i*size, 0.f, (float)k*size), Vector2(size, size) }
			);

			vertices.push_back(
			{ Vector3((float)i*size + size, 0.f, (float)k*size), Vector2(size, size) }
			);

			vertices.push_back(
			{ Vector3((float)i*size, 0.f, (float)k*size - size), Vector2(size, size) }
			);

			vertices.push_back(
			{ Vector3((float)i*size + size, 0.f, (float)k*size - size), Vector2(size, size) }
			);
		}
	}

	m_vtxBuffer.Create(m_renderer, vertices.size(), sizeof(sVertexType), &vertices[0]);

	if (!m_shader.Create(m_renderer, "../media/shader11/tessellation_quadtree.fxo", "Tech"
		, eVertexType::POSITION | eVertexType::TEXTURE0
		//| eVertexType::TEXTURE1
	))
	{
		assert(0);
	}

	m_cbMatrix.Create(m_renderer);
	m_cbTessellation.Create(m_renderer);
	m_texture.Create(m_renderer, "../media/sora1.jpg");

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	cAutoCam cam(&m_mainCamera);
	GetMainCamera().Update(deltaSeconds);
}


void cViewer::OnRender(const float deltaSeconds)
{
	cAutoCam cam(&m_mainCamera);

	GetMainLight().Bind(m_renderer);
	GetMainCamera().Bind(m_renderer);

	CommonStates states(m_renderer.GetDevice());
	//m_renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());
	//m_renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
	//m_renderer.GetDevContext()->OMSetBlendState(states.Opaque(), 0, 0xffffffff);
	if (m_isWireframe)
		m_renderer.GetDevContext()->RSSetState(states.Wireframe());
	else
		m_renderer.GetDevContext()->RSSetState(states.CullCounterClockwise());

	//m_renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);

	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();

		m_shader.SetTechnique("Tech");
		m_shader.Begin();
		m_shader.BeginPass(m_renderer, 0);

		Matrix44 world;
		m_cbMatrix.m_v->worldMatrix = XMMatrixTranspose(world.GetMatrixXM());
		m_cbMatrix.m_v->viewMatrix = XMMatrixTranspose(GetMainCamera().GetViewMatrix().GetMatrixXM());
		m_cbMatrix.m_v->projectionMatrix = XMMatrixTranspose(GetMainCamera().GetProjectionMatrix().GetMatrixXM());
		m_cbMatrix.Update(m_renderer, 0);

		m_cbTessellation.m_v->eyePos = GetMainCamera().GetEyePos();
		m_cbTessellation.m_v->tessellationAmount = m_tesellationAmount;
		m_cbTessellation.Update(m_renderer, 1);

		m_vtxBuffer.Bind(m_renderer);
		m_texture.Bind(m_renderer, 0);
		m_renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		m_renderer.GetDevContext()->Draw(m_vtxBuffer.GetVertexCount(), 0);

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
			m_isWireframe = !m_isWireframe;
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

		case VK_UP: m_tesellationAmount += 1.0f; break;
		case VK_DOWN: m_tesellationAmount -= 1.0f; break;
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

		cCamera *camera = &m_mainCamera;
		cAutoCam cam(camera);

		sf::Vector2i pos = { (int)LOWORD(lParam), (int)HIWORD(lParam) };

		const Ray ray = graphic::GetMainCamera().GetRay(pos.x, pos.y);
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

