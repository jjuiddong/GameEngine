// LoginServerApp.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "LoginServerApp.h"
#include "Src/LoginServer.h"
#include "Network/Service/ServerApp.h"

#include "NetProtocol/Src/login_Protocol.cpp"
#include "NetProtocol/Src/login_ProtocolListener.cpp"
#include "NetProtocol/Src/server_network_Protocol.cpp"
#include "NetProtocol/Src/server_network_ProtocolListener.cpp"
#include "NetProtocol/src/certify_Protocol.cpp"
#include "NetProtocol/src/certify_ProtocolListener.cpp"


using namespace network;


// CLoginServerApp
class CLoginServerApp : public network::CServerApp
{
public:
	CLoginServerApp();
	virtual ~CLoginServerApp();
	virtual bool OnInit();

	CLoginServer *m_pLoginSvr;
};

DECLARE_SERVERAPP(CLoginServerApp);
IMPLEMENT_SERVERAPP(CLoginServerApp);

CLoginServerApp::CLoginServerApp() :
	m_pLoginSvr(NULL)
{
	m_MemoryMonitorFilePath = "loginserver_monitor.json";
	m_NetworkConfigFilePath  = "loginserver_config.json";
	m_TitleName = "LoginServer";
	m_SvrType = "loginsvr";
	SetRect(&m_WindowRect, 0, 300, 200, 300);
}

CLoginServerApp::~CLoginServerApp()
{
	//SAFE_DELETE(m_pLoginSvr);
}


/**
 @brief 
 */
bool CLoginServerApp::OnInit()
{
	m_pLoginSvr = new CLoginServer();
	if (!network::ConnectDelegation("client", 	m_pLoginSvr))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: ConnectDelegation Fail !!\n" );
		return false;
	}
	if (!network::AddDelegation( "lobbysvr", new network::CSubServerPlug("loginsvr")))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: AddDelegation Fail !!\n" );
		return false;
	}
	if (!network::AddDelegation( "certifysvr", new network::CSubServerPlug("loginsvr")))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: AddDelegation Fail !!\n" );
		return false;
	}
	return true;
}
