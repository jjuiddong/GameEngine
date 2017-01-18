// RelayServerApp.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "RelayServerApp.h"
#include "Src/RelayServer.h"
#include "Network/Service/ServerApp.h"

using namespace network;


// CRelayServerApp
class CRelayServerApp : public network::CServerApp
{
public:
	CRelayServerApp();
	virtual ~CRelayServerApp();
	virtual bool OnInit();

	CRelayServer *m_pRelaySvr;
};

DECLARE_SERVERAPP(CRelayServerApp);
IMPLEMENT_SERVERAPP(CRelayServerApp);

CRelayServerApp::CRelayServerApp() :
	m_pRelaySvr(NULL)
{
	m_MemoryMonitorFilePath = "relayserver_monitor.json";
	m_NetworkConfigFilePath  = "relayserver_config.json";
	m_TitleName = "RelayServer";
	SetRect(&m_WindowRect, 200, 300, 200, 300);
}

CRelayServerApp::~CRelayServerApp()
{
	SAFE_DELETE(m_pRelaySvr);
}


/**
 @brief 
 */
bool CRelayServerApp::OnInit()
{
	m_pRelaySvr = new CRelayServer();
	if (!network::ConnectDelegation("client", 	m_pRelaySvr))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: ConnectDelegation Fail !!\n" );
		return false;
	}
	return true;
}
