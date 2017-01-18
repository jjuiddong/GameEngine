
#include "stdafx.h"
#include "LoginServer.h"
#include "Network/Service/SubServerPlug.h"

#include "Network/ProtocolHandler/BasicC2SHandler.h"
#include "BasicC2SHandler_LoginSvr.h"


using namespace network;


CLoginServer::CLoginServer() :
	m_pBasicPrtHandler(NULL)
{
}

CLoginServer::~CLoginServer()
{
	SAFE_DELETE(m_pBasicPrtHandler);

}


/**
 @brief OnConnectNetGroupController
 */
void	CLoginServer::OnConnectMultiPlug()
{
	CServer::OnConnectMultiPlug();

	MultiPlugPtr pLobbySvrController = multinetwork::CMultiNetwork::Get()->GetMultiPlug("lobbysvr");
	if (!pLobbySvrController)
	{
		clog::Error( clog::ERROR_CRITICAL, "CLoginServer Init Error!! not found lobbysvr multiplug" );
		return;
	}
	MultiPlugPtr pCertifySvrController = multinetwork::CMultiNetwork::Get()->GetMultiPlug("certifysvr");
	if (!pCertifySvrController)
	{
		clog::Error( clog::ERROR_CRITICAL, "CLoginServer Init Error!! not found certify multiplug" );
		return;
	}

	AddChild( pLobbySvrController );
	AddChild( pCertifySvrController );

	GetServer()->SetOption( true );

	RegisterProtocol(&m_BasicProtocol);
	RegisterProtocol(&m_LoginProtocol);

	m_pBasicPrtHandler = new CBasicC2SHandler_LoginSvr(*pCertifySvrController, *GetServer());
	AddProtocolListener(this);
	AddProtocolListener(m_pBasicPrtHandler);

	//pLobbySvrController->AddProtocolListener(this);
	//pCertifySvrController->AddProtocolListener(this);

	NETEVENT_CONNECT(EVT_CONNECT, CLoginServer, CLoginServer::OnSubServerConnect);
	NETEVENT_CONNECT(EVT_CONNECT, CLoginServer, CLoginServer::OnSubServerConnect);

}


/**
 @brief OnLobbySvrConnect
 */
void	CLoginServer::OnSubServerConnect(CNetEvent &event)
{
	MultiPlugPtr pLobbySvrController = multinetwork::CMultiNetwork::Get()->GetMultiPlug("lobbysvr");
	if (!pLobbySvrController)
	{
		clog::Error( clog::ERROR_CRITICAL, "OnSubServerConnect Error!! not found lobbysvr multiplug" );
		return;
	}
	MultiPlugPtr pCertifySvrController = multinetwork::CMultiNetwork::Get()->GetMultiPlug("certifysvr");
	if (!pCertifySvrController)
	{
		clog::Error( clog::ERROR_CRITICAL, "OnSubServerConnect Error!! not found certify multiplug" );
		return;
	}

	if (pLobbySvrController->IsConnect())
		pLobbySvrController->RegisterProtocol(&m_SvrNetworkProtocol);
	if (pCertifySvrController->IsConnect())
		pCertifySvrController->RegisterProtocol(&m_CertifyProtocol);
}


/**
 @brief ReqMovePlayer
 */
bool CLoginServer::ReqMovePlayer(server_network::ReqMovePlayer_Packet &packet)
{
	
	return true;
}


/**
 @brief AckMovePlayer
 */
bool CLoginServer::AckMovePlayer(server_network::AckMovePlayer_Packet &packet)
{
	CSession *pClient = CheckClientId(GetServer(), packet.id, 0, NULL, NULL);
	if (!pClient)
	{
		clog::Error( clog::ERROR_CRITICAL, "AckMovePlayer Error!! not found client id = %s", packet.id.c_str() );
		m_SvrNetworkProtocol.ReqMovePlayerCancel( packet.senderId, SEND_T, packet.id );
		return false;
	}

	if (error::ERR_SUCCESS == packet.errorCode)
	{
		m_BasicProtocol.AckMoveToServer( pClient->GetNetId(), SEND_T, error::ERR_SUCCESS, "lobbysvr", packet.ip, packet.port);
	}
	else
	{
		m_BasicProtocol.AckMoveToServer( pClient->GetNetId(), SEND_T, packet.errorCode, "lobbysvr", " ", 0);
	}
	return true;
}


/**
 @brief ReqLobbyIn
 로비서버로 이동 요청 
 */
bool CLoginServer::ReqLobbyIn(login::ReqLobbyIn_Packet &packet)
{
	CSession *pClient = CheckClientNetId(GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher);
	RETV(!pClient, false);

	if (!CheckClientConnection(pClient, &m_BasicProtocol, packet.pdispatcher))
		return false;

	MultiPlugDelegationPtr pLobbySvrDelegation = CheckDelegation("lobbysvr", packet.senderId, 
		&m_BasicProtocol, packet.pdispatcher);
	RETV(!pLobbySvrDelegation, false);
	//MultiPlugDelegationPtr pLobbySvrDelegation = multinetwork::CMultiNetwork::Get()->GetDelegation("lobbysvr");
	//if (!pLobbySvrDelegation)
	//{
		//clog::Error( clog::ERROR_CRITICAL, "ReqLobbyIn Error!! not found lobbysvr netgroupdelegation" );
		//m_LoginProtocol.AckLobbyIn( packet.senderId, SEND_T, error::ERR_REQLOBBYIN_NOTFOUND_SERVER );
	//	return false;
	//}

	CSubServerPlug *pSubSvrPlug = CheckCasting<CSubServerPlug*>(pLobbySvrDelegation.Get(), packet.senderId, 
		&m_BasicProtocol, packet.pdispatcher);
	RETV(!pSubSvrPlug, false);
	//CSubServerPlug *pSubSvrCon = dynamic_cast<CSubServerPlug*>(pLobbySvrDelegation.Get());
	//if (!pSubSvrCon)
	//{
	//	clog::Error( clog::ERROR_CRITICAL, "ReqLobbyIn Error!! CSubServerConnector convert error" );
	//	m_LoginProtocol.AckLobbyIn( packet.senderId, SEND_T, error::ERR_REQLOBBYIN_NOTFOUND_SERVER );
	//	return false;
	//}

	std::list<SSubServerInfo> subServers = pSubSvrPlug->GetSubServerInfo();
	subServers.sort();
	if (subServers.empty())
	{
		clog::Error( clog::ERROR_CRITICAL, "ReqLobbyIn Error!! not found lobbysvr server" );
		m_LoginProtocol.AckLobbyIn( packet.senderId, SEND_T, error::ERR_REQLOBBYIN_NOTFOUND_SERVER );
		return false;
	}

	SSubServerInfo targetSvr = subServers.front();
	pSubSvrPlug->RegisterProtocol( &m_SvrNetworkProtocol );
	m_SvrNetworkProtocol.ReqMovePlayer( targetSvr.serverId, SEND_T, 
		pClient->GetName(), pClient->GetCertifyKey(), 0, targetSvr.ip, targetSvr.portnum  );

	return true; 
}
