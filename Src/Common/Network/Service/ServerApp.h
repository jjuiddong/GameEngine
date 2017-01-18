/**
Name:   ServerApp.h
Author:  jjuiddong
Date:    5/1/2013

	Server App
*/
#pragma once

namespace network
{

	class CServerApp
	{
	public:
		CServerApp();
		virtual ~CServerApp() {}
		virtual bool OnInit()=0;
		virtual std::string ToString() { return ""; }
		static CServerApp*GetInstance() { return m_pInstance; }
		static void SetInstance(CServerApp*ptr) { m_pInstance = ptr; }
		std::string m_SvrType;
		std::string m_TitleName;
		std::string m_MemoryMonitorFilePath;
		std::string m_NetworkConfigFilePath;
		RECT m_WindowRect;
	private:
		static CServerApp* m_pInstance;
	};

}


#define IMPLEMENT_SERVERAPP( appName )	\
	appName& GetServerApp() { return *static_cast<appName*>(network::CServerApp::GetInstance()); }    \
	network::CServerApp* CreateServerApp() {				\
	network::CServerApp::SetInstance(new appName);\
	return network::CServerApp::GetInstance();			\
}


#define DECLARE_SERVERAPP( appName ) \
	extern appName& GetServerApp();\
	int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) \
	{ return _WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow); }


// WinMain
int _WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow );
