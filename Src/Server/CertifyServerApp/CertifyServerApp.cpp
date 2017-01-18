// CertifyServerApp.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "CertifyServerApp.h"
#include "Src/CertifyServer.h"
#include "Network/Service/ServerApp.h"

#include "NetProtocol/Src/server_network_ProtocolListener.cpp"

using namespace network;


// CCertifyServerApp
class CCertifyServerApp : public network::CServerApp
{
public:
	CCertifyServerApp();
	virtual ~CCertifyServerApp();
	virtual bool OnInit();

	CCertifyServer *m_pCertifySvr;
};

DECLARE_SERVERAPP(CCertifyServerApp);
IMPLEMENT_SERVERAPP(CCertifyServerApp);

CCertifyServerApp::CCertifyServerApp() :
	m_pCertifySvr(NULL)
{
	m_MemoryMonitorFilePath = "certifyserver_monitor.json";
	m_NetworkConfigFilePath  =  "certifyserver_config.json";
	m_TitleName = "CertifyServer";
	SetRect(&m_WindowRect, 200, 0, 200, 300);
}

CCertifyServerApp::~CCertifyServerApp()
{
	SAFE_DELETE(m_pCertifySvr);
}


/**
 @brief 
 */
bool CCertifyServerApp::OnInit()
{
	m_pCertifySvr = new CCertifyServer();
	if (!network::ConnectDelegation("p2p", m_pCertifySvr))
	{
		clog::Error( clog::ERROR_CRITICAL, "network :: ConnectDelegation Fail !!\n" );
		//return false;
	}
	return true;
}
