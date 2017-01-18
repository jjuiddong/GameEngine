
#include "stdafx.h"
#include "ServerApp.h"

MEMORYMONITOR_INNER_PROCESS();

namespace network
{
	#define MAX_LOADSTRING 100
	HINSTANCE hInst;
	std::string szTitle = "ServerApp";
	std::string szWindowClass = "CServerApp";
	RECT windowRect;
	HWND g_Hwnd = NULL;

	// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
	ATOM				MyRegisterClass(HINSTANCE hInstance);
	BOOL				InitInstance(HINSTANCE, int);
	LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
}

using namespace network;

CServerApp* CServerApp::m_pInstance = NULL;
CServerApp* CreateServerApp();


CServerApp::CServerApp() 
{
	SetRect(&m_WindowRect, 0, 0, 200, 300);
}

/**
 @brief _tWinMain
 */
int _WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CServerApp *pApp = CreateServerApp();
	if (!pApp)
		return 0;
	
	if (!pApp->m_TitleName.empty())
		szTitle = pApp->m_TitleName;
	windowRect = pApp->m_WindowRect;

	MyRegisterClass(hInstance);
	if (!InitInstance (hInstance, nCmdShow))
	{
		goto exit;
	}

	// init memmonitor
	if (!pApp->m_MemoryMonitorFilePath.empty())
	{
		if (!memmonitor::Init( memmonitor::INNER_PROCESS, hInstance, pApp->m_MemoryMonitorFilePath))
		{
			clog::Error(log::ERROR_CRITICAL, "memmonitor::init() fail !!\n" );
			goto exit;
		}
	}

	// init network
	if (!pApp->m_NetworkConfigFilePath.empty())
	{
		if (!network::Init(1, pApp->m_NetworkConfigFilePath))
		{
			clog::Error(log::ERROR_CRITICAL, "network::init() fail !!\n" );
			goto exit;
		}
	}

	// OnInit App
	if (!pApp->OnInit())
		goto exit;


	network::StartMultiNetwork();


	MSG msg;
	int tick = GetTickCount();
	bool bDoingBackgroundProcessing = true;
	while ( bDoingBackgroundProcessing ) 
	{ 
		while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
		{ 
			if ( !GetMessage(&msg, NULL, 0, 0)) 
			{ 
				bDoingBackgroundProcessing = FALSE; 
				break; 
			}
			if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		const int curT = GetTickCount();
		if (curT - tick > 1000)
		{
			InvalidateRect(g_Hwnd,NULL,TRUE);
			tick = curT;
		}
		Sleep(1);
	}

exit:
	SAFE_DELETE(pApp);
	network::Clear();
	memmonitor::Cleanup();
	return 0;
}


ATOM network::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = L"";
	wcex.lpszClassName	= L"CServerApp";
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

BOOL network::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다
	hWnd = CreateWindow(common::str2wstr(szWindowClass).c_str(), 
		common::str2wstr(szTitle).c_str(), WS_OVERLAPPEDWINDOW,
		windowRect.left, windowRect.top, windowRect.right, windowRect.bottom,
		NULL, NULL, hInstance, NULL);
	g_Hwnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


//------------------------------------------------------------------------
// 서버정보 출력
//------------------------------------------------------------------------
int PrintString(HDC hdc, int x, int y, const std::string &str)
{
	int fi = 0, li=0;
	while (1)
	{
		li = str.find('\n', fi);
		if (li < 0)
			break;
		std::string s0 = str.substr(fi, li-fi);
		TextOutA( hdc, x, y, s0.c_str(), s0.size() );
		y += 16;
		fi = li+1;
	}
	return y;
}


void Paint(HDC hdc)
{
	if (CServerApp::GetInstance())
	{
		std::string str1 = CServerApp::GetInstance()->ToString();
		std::string str2 = network::ToString();
		int y = PrintString(hdc, 10, 0, str1);
		y += PrintString(hdc, 10, y+16, str2);
	}
}


LRESULT CALLBACK network::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Paint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_F7:
				{
					memmonitor::ShowToggle();
				}
				break;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

