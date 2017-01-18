// FarmServerApp.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "FarmServerApp.h"
#include "Src/FarmServer.h"
#include "Network/Service/ServerApp.h"

using namespace network;


// CFarmServerApp
class CFarmServerApp : public network::CServerApp
{
public:
	CFarmServerApp();
	virtual ~CFarmServerApp();
	virtual bool OnInit() override;

	CFarmServer *m_pFarmSvr;
};

DECLARE_SERVERAPP(CFarmServerApp);
IMPLEMENT_SERVERAPP(CFarmServerApp);

CFarmServerApp::CFarmServerApp() :
	m_pFarmSvr(NULL)
{
	m_MemoryMonitorFilePath = "farmserver_monitor.json";
	m_NetworkConfigFilePath  =   "farmserver_config.json";
	m_TitleName = "FarmServer";
	SetRect(&m_WindowRect, 0, 0, 200, 300);
}

CFarmServerApp::~CFarmServerApp()
{
	//SAFE_DELETE(m_pFarmSvr);
}


/**
 @brief 
 */
bool CFarmServerApp::OnInit()
{
	m_pFarmSvr = new CFarmServer();
	if (!network::ConnectDelegation("client", 	m_pFarmSvr))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: ConnectDelegation Fail !!\n" );
		return false;
	}
	return true;
}
