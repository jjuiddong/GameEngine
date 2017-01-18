// ServerApp.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LobbyServerApp.h"
#include "Lib/ChatServer.h"
#include "Lib/ServerLauncher.h"
#include "Lib/LobbyServer.h"

#include "NetProtocol/Src/login_Protocol.cpp"
#include "NetProtocol/Src/login_ProtocolListener.cpp"
#include "NetProtocol/Src/server_network_Protocol.h"
#include "NetProtocol/Src/server_network_ProtocolListener.h"
#include "NetProtocol/Src/server_network_Protocol.cpp"
#include "NetProtocol/Src/server_network_ProtocolListener.cpp"
#include "NetProtocol/src/certify_Protocol.cpp"
#include "NetProtocol/src/certify_ProtocolListener.cpp"



// CLobbyServerApp
class CLobbyServerApp : public CServerApp
{
public:
	CLobbyServerApp();
	virtual ~CLobbyServerApp();
	virtual bool OnInit() override;
	virtual std::string ToString() override;

	CLobbyServer *m_pLobbyServer;
};

DECLARE_SERVERAPP(CLobbyServerApp);
IMPLEMENT_SERVERAPP(CLobbyServerApp);

CLobbyServerApp::CLobbyServerApp() :
	m_pLobbyServer(NULL)
{
	m_MemoryMonitorFilePath = "madsoccer_server_monitor.json" ;
	m_NetworkConfigFilePath  = "madsoccer_server_config.json";
	m_TitleName = "LobbyServer";
	m_SvrType = "lobbysvr";
	SetRect(&m_WindowRect, 400, 0, 200, 300);
}

CLobbyServerApp::~CLobbyServerApp()
{
	//SAFE_DELETE(m_pLobbyServer);
}

std::string CLobbyServerApp::ToString()
{
	if (m_pLobbyServer)
		return m_pLobbyServer->ToString();
	return "";
}


/**
 @brief 
 */
bool CLobbyServerApp::OnInit()
{
	m_pLobbyServer = new CLobbyServer();
	if (!network::AddDelegation( "client", m_pLobbyServer ))
	{
		clog::Error(log::ERROR_CRITICAL, "network::ConnectDelegation() fail !!\n" );
		return false;
	}
	if (!network::AddDelegation( "loginsvr", new network::CSubServerPlug("lobbysvr")))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: AddDelegation Fail !! loginsvr\n" );
		return false;
	}
	if (!network::AddDelegation( "certifysvr", new network::CSubServerPlug("lobbysvr")))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: AddDelegation Fail !! certifysvr\n" );
		return false;
	}
	if (!network::AddDelegation( "gamesvr", new network::CSubServerPlug("lobbysvr")))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: AddDelegation Fail !! gamesvr\n" );
		return false;
	}

	return true;
}
