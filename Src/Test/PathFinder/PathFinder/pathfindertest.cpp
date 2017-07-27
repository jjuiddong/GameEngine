//
// PathFinder Test
//

#include "stdafx.h"
#include "../../../../../Common/AI/path/pathfinder.h"

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
	sf::Vector2i m_curPos;
	float m_moveLen;
	Plane m_groundPlane1;
	cGrid m_ground;
	cModel2 m_model;
	cDbgBox m_box;
	cDbgLine m_line;
	cDbgLineList m_lineList;
	Vector3 m_startPos;
	Vector3 m_endPos;
	cText m_text;
	cSprite m_sprite;
	cSurface2 m_surface;

	ai::cPathFinder m_pathFinder;


	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
};

INIT_FRAMEWORK(cViewer);


cViewer::cViewer()
	: m_groundPlane1(Vector3(0, 1, 0), 0)
{
	m_windowName = L"PathFinder Test";
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

	cResourceManager::Get()->SetMediaDirectory("../media/");

	const int WINSIZE_X = m_windowRect.right - m_windowRect.left;
	const int WINSIZE_Y = m_windowRect.bottom - m_windowRect.top;
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(30, 10, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 0.1f, 10000.0f);
	GetMainCamera()->SetViewPort(WINSIZE_X, WINSIZE_Y);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL,
		Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
		Vector4(0.2f, 0.2f, 0.2f, 1));
	const Vector3 lightPos(-300, 300, -300);
	const Vector3 lightLookat(0, 0, 0);
	GetMainLight().SetPosition(lightPos);
	GetMainLight().SetDirection((lightLookat - lightPos).Normal());

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	//m_gui.Init(m_hWnd, m_renderer.GetDevice());
	//m_gui.SetContext();

	m_model.Create(m_renderer, 0, "ChessBishop.x");
	Matrix44 S;
	S.SetScale(Vector3(80, 80, 80));
	m_model.m_tm = S;

	m_box.InitBox(m_renderer);
	m_box.SetColor(0);
	m_line.InitCube(m_renderer);

	m_ground.Create(m_renderer, 10, 10, 100);
	//m_ground.m_tex = cResourceManager::Get()->LoadTexture(m_renderer, "../media/terrain/¹Ù´Ú.jpg");

	m_pathFinder.Create(200);

	Vector3 offset[] = {
		Vector3(0,0,0)
		, Vector3(70,0,70)
		, Vector3(-70, 0, 70)
		, Vector3(70, 0, -70)
		, Vector3(-70, 0, -70)
	};

	int offsetIndex = 0;
	for (int o = 0; o < 5; ++o)
	{
		for (int x = 0; x < 5; ++x)
		{
			for (int z = 0; z < 5; ++z)
			{
				ai::sVertex vtx;
				vtx.pos = Vector3(x * 10.f, 0, z * 10.f) + offset[o];
				for (int i = 0; i < 4; ++i)
					vtx.edge[i] = -1;

				int idx = 0;
				if (x-1 >= 0)
					vtx.edge[idx++] = z + (x-1)*5 + offsetIndex;
				if (x + 1 < 5)
					vtx.edge[idx++] = z + (x + 1) * 5 + offsetIndex;
				if (z - 1 >= 0)
					vtx.edge[idx++] = z-1 + x*5 + offsetIndex;
				if (z + 1 < 5)
					vtx.edge[idx++] = z+1 + x*5 + offsetIndex;

				m_pathFinder.AddVertex(vtx);
			}
		}

		offsetIndex += 25;
	}


	m_pathFinder.m_vertices[24].edge[2] = 25;
	m_pathFinder.m_vertices[25].edge[2] = 24;
	m_pathFinder.m_vertices[4].edge[2] = 70;
	m_pathFinder.m_vertices[70].edge[2] = 4;
	m_pathFinder.m_vertices[20].edge[2] = 79;
	m_pathFinder.m_vertices[79].edge[2] = 20;
	m_pathFinder.m_vertices[0].edge[2] = 124;
	m_pathFinder.m_vertices[124].edge[2] = 0;

	m_startPos = Vector3(0, 0, 0);
	m_endPos = Vector3(90, 0, 40);

	m_lineList.Create(m_renderer, 1000);
	m_lineList.ClearLines();
	for (auto &vtx : m_pathFinder.m_vertices)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (vtx.edge[i] < 0)
				break;

			m_lineList.AddLine(
				vtx.pos
				, m_pathFinder.m_vertices[vtx.edge[i]].pos);
		}
	}
	m_lineList.Render(m_renderer);


	m_surface.Create(m_renderer, 128, 128, 1);
	m_text.Create(m_renderer);
	m_text.SetText("test");

	m_surface.Begin(m_renderer);
	if (SUCCEEDED(m_renderer.GetDevice()->Clear(
		0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(0, 50, 50, 50),
		1.0f, 0)))
	{
		m_text.SetPos(9, 10);
		m_text.SetColor(D3DCOLOR_XRGB(0, 0, 0));
		m_text.Render();
		m_text.SetPos(9, 9);
		m_text.Render();
		m_text.SetPos(10, 9);
		m_text.Render();

		m_text.SetColor(D3DCOLOR_XRGB(255, 255, 255));
		m_text.SetPos(10, 10);
		m_text.Render();

	}
	m_surface.End(m_renderer);


	return true;
}


void cViewer::OnUpdate(const float deltaSeconds)
{
	m_model.Update(m_renderer, deltaSeconds);

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


void cViewer::OnRender(const float deltaSeconds)
{
	// Render
	if (m_renderer.ClearScene())
	{
		m_renderer.BeginScene();
		m_renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix44::Identity);

		GetMainCamera()->Bind(*m_model.m_shader);
		GetMainLight().Bind(*m_model.m_shader);

		m_ground.Render(m_renderer);

		for (auto &vtx : m_pathFinder.m_vertices)
		{
			Matrix44 tm;
			tm.SetPosition(vtx.pos);
			m_box.Render(m_renderer, tm);
		}

		m_lineList.Render(m_renderer);

		vector<Vector3> path;
		m_pathFinder.Find(m_startPos, m_endPos, path);
		for (u_int i = 1; i < path.size(); ++i)
		{
			m_line.SetLine(path[i - 1], path[i], 0.3f);
			m_line.Render(m_renderer);

		}

		m_model.RenderShader(m_renderer);
		m_surface.Render(m_renderer);

		m_renderer.RenderFPS();
		m_renderer.RenderAxis();

		m_renderer.EndScene();
		m_renderer.Present();
	}
}


void cViewer::OnLostDevice()
{
}


void cViewer::OnShutdown()
{
}


void cViewer::ChangeWindowSize()
{
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
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

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
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
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

		for (auto &vtx : m_pathFinder.m_vertices)
		{
			cBoundingBox bbox;
			bbox.SetBoundingBox(vtx.pos - Vector3(1, 1, 1)
				, vtx.pos + Vector3(1, 1, 1));
			if (bbox.Pick(orig, dir))
			{
				m_startPos = vtx.pos;
				break;
			}
		}
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

		Vector3 orig, dir;
		graphic::GetMainCamera()->GetRay(m_curPos.x, m_curPos.y, orig, dir);

		for (auto &vtx : m_pathFinder.m_vertices)
		{
			cBoundingBox bbox;
			bbox.SetBoundingBox(vtx.pos - Vector3(1, 1, 1)
				, vtx.pos + Vector3(1, 1, 1));
			if (bbox.Pick(orig, dir))
			{
				m_endPos = vtx.pos;
				break;
			}
		}
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
	}
	break;
	}
}

