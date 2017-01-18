// GameServerApp.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "GameServerApp.h"
#include "Src/GameServer.h"
#include "Network/Service/ServerApp.h"

#include "NetProtocol/src/server_network_Protocol.cpp"
#include "NetProtocol/Src/server_network_ProtocolListener.cpp"


using namespace network;


// CGameServerApp
class CGameServerApp : public network::CServerApp
{
public:
	CGameServerApp();
	virtual ~CGameServerApp();
	virtual bool OnInit();

	CGameServer *m_pGameSvr;
};

DECLARE_SERVERAPP(CGameServerApp);
IMPLEMENT_SERVERAPP(CGameServerApp);

CGameServerApp::CGameServerApp() :
	m_pGameSvr(NULL)
{
	m_MemoryMonitorFilePath = "gameserver_monitor.json";
	m_NetworkConfigFilePath  =  "gameserver_config.json";
	m_TitleName = "GameServer";
	SetRect(&m_WindowRect, 600, 0, 200, 300);
}

CGameServerApp::~CGameServerApp()
{
	//SAFE_DELETE(m_pGameSvr);
}


/**
 @brief 
 */
bool CGameServerApp::OnInit()
{
	if (!network::AddDelegation("lobbysvr", 	new CSubServerPlug("gamesvr")))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: ConnectDelegation Fail !! lobbysvr \n" );
		return false;
	}

	m_pGameSvr = new CGameServer();
	if (!network::AddDelegation("client", 	m_pGameSvr))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: ConnectDelegation Fail !!\n" );
		return false;
	}

	return true;
}
