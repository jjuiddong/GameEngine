//
// Collada Viewer
//

#include "stdafx.h"
using namespace graphic;


class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float deltaSeconds) override;
	virtual void OnRender(const float deltaSeconds) override;
	virtual void OnDeviceLost() override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void ChangeWindowSize();


private:
	cImGui m_gui;

	cCube m_cube1;
	cCube3 m_cube2;
	Matrix44 m_rotateTm;
	Matrix44 m_rotateTm2;
	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);


cViewer::cViewer()
{
	m_windowName = L"Collada Viewer";
	const RECT r = { 0, 0, 1024, 768 };
	//const RECT r = { 0, 0, 1280, 1024};
	m_windowRect = r;
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

	cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = 1024;
	const int WINSIZE_Y = 768;
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.f, 10000.0f);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	GetMainLight().SetPosition(Vector3(-30000, 30000, -30000));
	GetMainLight().SetDirection(Vector3(1, -1, 1).Normal());
	//GetMainLight().SetDirection(Vector3(0, -1, 0).Normal());

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	m_cube1.InitCube(m_renderer);
	m_cube1.SetColor(D3DXCOLOR(1, 0, 0, 1));

	m_cube2.InitCube(m_renderer);
	m_cube2.m_tm.SetTranslate(Vector3(3.5f, -2, 2));
	m_cube2.m_mtrl.InitBlue();

	m_gui.Init(m_hWnd, m_renderer.GetDevice());
	m_gui.SetContext();
	g_root.Init(m_renderer);

	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	g_root.Update(m_renderer, deltaSeconds);

	// keyboard
	if (GetFocus() == m_hWnd)
	{
		const float vel = 10 * deltaSeconds;
		if (GetAsyncKeyState('W'))
			GetMainCamera()->MoveFront(vel);
		else if (GetAsyncKeyState('A'))
			GetMainCamera()->MoveRight(-vel);
		else if (GetAsyncKeyState('D'))
			GetMainCamera()->MoveRight(vel);
		else if (GetAsyncKeyState('S'))
			GetMainCamera()->MoveFront(-vel);
		else if (GetAsyncKeyState('E'))
			GetMainCamera()->MoveUp(vel);
		else if (GetAsyncKeyState('C'))
			GetMainCamera()->MoveUp(-vel);

		GetMainCamera()->Update(deltaSeconds);
	}
}

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);

void cViewer::OnRender(const float deltaSeconds)
{
	GetMainLight().Bind(m_renderer, 0);

	m_gui.SetContext();
	m_gui.NewFrame();

	g_root.PreRender(m_renderer, deltaSeconds);

	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();
		m_renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix44::Identity);
		m_renderer.RenderGrid();
		m_renderer.RenderAxis();
		m_renderer.RenderFPS();

		g_root.Render(m_renderer, deltaSeconds);

		m_gui.Render();
		m_renderer.EndScene();
		m_renderer.Present();
	}
}


void cViewer::OnDeviceLost()
{
	m_gui.InvalidateDeviceObjects();
	m_renderer.ResetDevice(0, 0, true);
	m_gui.CreateDeviceObjects();
}


void cViewer::OnShutdown()
{
	m_gui.Shutdown();
}


void cViewer::ChangeWindowSize()
{
	if (m_renderer.CheckResetDevice())
	{
		m_gui.InvalidateDeviceObjects();
		g_root.LostDevice();
		m_renderer.ResetDevice();
		m_gui.CreateDeviceObjects();
		g_root.ResetDevice(m_renderer);
	}
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	cInputManager::Get()->MouseProc(message, wParam, lParam);

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

		g_root.m_model.Create(m_renderer, filePath, "", "", true, true);
	}
	break;

	case WM_MOUSEWHEEL:
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		//dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera()->GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera()->Zoom((zDelta<0) ? -zoomLen : zoomLen);
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
		{
			static bool flag = false;
			m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case VK_RETURN:
			cResourceManager::Get()->ReloadShader(m_renderer);
			g_root.UpdateShader();
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
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
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (wParam & 0x10) // middle button down
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
		}

		if (m_LButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			Quaternion q1(graphic::GetMainCamera()->GetRight(), -y * 0.01f);
			Quaternion q2(graphic::GetMainCamera()->GetUpVector(), -x * 0.01f);

			if (GetAsyncKeyState('F'))
			{
				//m_cube2.m_tm *= (q2.GetMatrix() * q1.GetMatrix());
			}
			else
			{
				m_rotateTm *= (q2.GetMatrix() * q1.GetMatrix());
			}

		}
		else if (m_RButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			//graphic::GetMainCamera()->Yaw2(x * 0.005f);
			//graphic::GetMainCamera()->Pitch2(y * 0.005f);

			graphic::GetMainCamera()->Yaw3(x * 0.005f, Vector3(0,0,0));
			graphic::GetMainCamera()->Pitch3(y * 0.005f, Vector3(0, 0, 0));

		}
		else if (m_MButtonDown)
		{
			const POINT point = { LOWORD(lParam), HIWORD(lParam) };
			const POINT pos = { point.x - m_curPos.x, point.y - m_curPos.y };
			m_curPos = point;

			const float len = graphic::GetMainCamera()->GetDistance();
			graphic::GetMainCamera()->MoveRight(-pos.x * len * 0.001f);
			graphic::GetMainCamera()->MoveUp(pos.y * len * 0.001f);
		}
		else
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
		}

	}
	break;
	}
}

