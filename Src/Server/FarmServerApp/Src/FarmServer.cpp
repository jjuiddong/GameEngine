
#include "stdafx.h"
#include "FarmServer.h"
#include "RemoteSubServer.h"
#include "SubServerGroup.h"
#include "Network/Utility/ServerAccess.h"
#include "FarmServerUtility.h"

using namespace network;

CFarmServer::CFarmServer()
{
}

CFarmServer::~CFarmServer()
{
}


/**
 @brief NetGroupController 와 연결되면 호출된다.
 */
void	CFarmServer::OnConnectMultiPlug()
{
	RegisterProtocol( &m_Protocol );
	AddProtocolListener(this);
	GetMultiPlug()->SetSessionFactory( new CRemoteSubServerFactory() );
	GetMultiPlug()->SetGroupFactory( new CSubServerGroupFactory() );

	m_pServer = GetServer();

	if (!farmsvr::ReadServerGrouprConfig( "servergroup_config.json", *this))
	{ // Error!!
		assert(0);
	}

	//NETEVENT_CONNECT( EVT_CLIENT_JOIN, CFarmServer, CFarmServer::OnClientJoin );

}


/**
 @brief 
 */
SubServerGroupPtr CFarmServer::FindGroup(const std::string &svrType)
{
	BOOST_FOREACH(auto &grp, GetServer()->GetRootGroup().GetChildren())
	{
		CSubServerGroup *pGroup = dynamic_cast<CSubServerGroup*>(grp);
		if (!pGroup)
			continue;
		if (pGroup->GetSvrType() == svrType)
			return pGroup;
	}
	return NULL;
}


/**
@brief ReqSubServerLogin
*/
bool CFarmServer::ReqSubServerLogin(farm::ReqSubServerLogin_Packet &packet)
{
	GroupPtr pFromGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pFromGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqSubServerLogin Error!!, not exist group user id = %d\n\n", packet.senderId );
		m_Protocol.AckSubServerLogin( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP);
		return false;
	}

	SubServerGroupPtr pSvrGroup = FindGroup(packet.svrType);
	if (!pSvrGroup)
	{
		//SubServerGroupPtr pNewGroup = dynamic_cast<CSubServerGroup*>(
		//	GetServer()->GetRootGroup().AddChild( GetServer()->GetGroupFactory()) );
		//if (!pNewGroup)
		{// Error!!
			clog::Error( log::ERROR_PROBLEM, "ReqSubServerLogin Error!!, not exist group %s \n\n", packet.svrType.c_str() );
			m_Protocol.AckSubServerLogin( packet.senderId, SEND_T, error::ERR_NO_CREATE_GROUP);
			return false;
		}
		//pSvrGroup = pNewGroup;
	}

	// Waiting Group -> New Group
	pFromGroup->RemovePlayer( pFromGroup->GetNetId(), packet.senderId );
	if (!pSvrGroup->AddPlayer(pSvrGroup->GetNetId(), packet.senderId))
	{ // Error!!
		pFromGroup->AddPlayer(pFromGroup->GetNetId(), packet.senderId); // 복구

		clog::Error( log::ERROR_PROBLEM, "ReqSubServerLogin Error!!, not join group\n" );
		m_Protocol.AckSubServerLogin( packet.senderId, SEND_T, error::ERR_NOT_JOIN_GROUP);
		return false;
	}
	pSvrGroup->AddViewer( GetServer()->GetRootGroup().GetNetId() );

	m_Protocol.AckSubServerLogin( packet.senderId, SEND_T, error::ERR_SUCCESS);
	return true;
}


/**
 @brief SendSubServerP2PCLink
 */
bool CFarmServer::SendSubServerP2PCLink(farm::SendSubServerP2PCLink_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "SendSubServerP2PCLink Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerP2PCLink( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP);
		return false;
	}

	RemoteSubServerPtr pClient = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pClient)
	{
		clog::Error( log::ERROR_PROBLEM, "SendSubServerP2PCLink Error!!, not exist user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerP2PCLink( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER );
		return false;
	}

	pClient->SetP2PCLink( packet.v );
	m_Protocol.AckSendSubServerP2PCLink( packet.senderId, SEND_T, error::ERR_SUCCESS );
	return true;
}


/**
 @brief SendSubServerP2PSLink
 */
bool CFarmServer::SendSubServerP2PSLink(farm::SendSubServerP2PSLink_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "SendSubServerP2PSLink Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerP2PSLink( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP);
		return false;
	}	

	RemoteSubServerPtr pClient = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pClient)
	{
		clog::Error( log::ERROR_PROBLEM, "SendSubServerP2PSLink Error!!, not exist user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerP2PSLink( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER );
		return false;
	}

	pClient->SetP2PSLink( packet.v );
	m_Protocol.AckSendSubServerP2PSLink( packet.senderId, SEND_T, error::ERR_SUCCESS );
	return true;
}


/**
 @brief SendSubServerInputLink
 */
bool CFarmServer::SendSubServerInputLink(farm::SendSubServerInputLink_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "SendSubServerInputLink Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerInputLink( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP);
		return false;
	}	

	RemoteSubServerPtr pClient = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pClient)
	{
		clog::Error( log::ERROR_PROBLEM, "SendSubServerInputLink Error!!, not exist user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerInputLink( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER );
		return false;
	}

	pClient->SetInputLink( packet.v );
	m_Protocol.AckSendSubServerInputLink( packet.senderId, SEND_T, error::ERR_SUCCESS );
	return true;
}


/**
 @brief SendSubServerOutputLink
 */
bool CFarmServer::SendSubServerOutputLink(farm::SendSubServerOutputLink_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "SendSubServerOutputLink Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerOutputLink(packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP);
		return false;
	}	

	RemoteSubServerPtr pClient = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pClient)
	{
		clog::Error( log::ERROR_PROBLEM, "SendSubServerOutputLink Error!!, not exist user id = %d\n", packet.senderId );
		m_Protocol.AckSendSubServerOutputLink( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER );
		return false;
	}

	pClient->SetOutputLink( packet.v );
	m_Protocol.AckSendSubServerOutputLink( packet.senderId, SEND_T, error::ERR_SUCCESS );
	return true;
}


/**
 @brief ReqServerInfoList
 */
bool CFarmServer::ReqServerInfoList(farm::ReqServerInfoList_Packet &packet)
{
	std::vector<network::SHostInfo> hostInfo;
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqServerInfoList Error!!, not exist group user id = %d\n", packet.senderId );
		packet.pdispatcher->PrintThisPacket(clog::LOG_F_N_O, "Error!! ");
		m_Protocol.AckServerInfoList( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.clientSvrType, packet.serverSvrType, hostInfo);
		return false;
	}

	SubServerGroupPtr pServerGroup = FindGroup( packet.serverSvrType );
	if (!pServerGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqServerInfoList Error!!, not exist serverSvrType group = %s\n", packet.serverSvrType.c_str() );
		packet.pdispatcher->PrintThisPacket(clog::LOG_F_N_O, "Error!! ");
		m_Protocol.AckServerInfoList( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.clientSvrType, packet.serverSvrType, hostInfo);
		return false;
	}

	hostInfo.reserve(10);
	pServerGroup->GetServerInfoCorrespondClient(packet.clientSvrType, packet.serverSvrType, CServerSessionAccess(GetServer()), hostInfo);

	if (hostInfo.empty())
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqServerInfoList Error!!, not found ServerInfo svrType = %s\n", packet.clientSvrType.c_str() );
		packet.pdispatcher->PrintThisPacket(clog::LOG_F_N_O, "Error!! ");
	}

	m_Protocol.AckServerInfoList( packet.senderId, SEND_T, 
		(hostInfo.size() <= 0)? error::ERR_REQSERVERINFO_NOTFOUND_SERVER : error::ERR_SUCCESS, 
		packet.clientSvrType, packet.serverSvrType, hostInfo );
	return true;
}


/**
 @brief ReqToBindOuterPort
 */
bool CFarmServer::ReqToBindOuterPort(farm::ReqToBindOuterPort_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqToBindOuterPort Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckToBindOuterPort( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType, 0);
		return false;
	}

	SubServerGroupPtr pSubSvrGroup = dynamic_cast<CSubServerGroup*>(pGroup.Get());
	if (!pSubSvrGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqToBindOuterPort Error!!, not convert group user id = %d\n", packet.senderId );
		m_Protocol.AckToBindOuterPort( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType, 0);
		return false;
	}

	const int bindPort = pSubSvrGroup->GetToBindOuterPort( CServerSessionAccess(GetServer()) );

	RemoteSubServerPtr pSubServer = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pSubServer)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqToBindOuterPort Error!!, not found user user id = %d\n", packet.senderId );
		m_Protocol.AckToBindOuterPort( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.bindSubServerSvrType, 0);
		return false;
	}

	pSubServer->SetOuterBindPort( "client", bindPort );
	m_Protocol.AckToBindOuterPort( packet.senderId, SEND_T, error::ERR_SUCCESS, packet.bindSubServerSvrType, bindPort );
	return true;
}


/**
 @brief ReqToBindInnerPort
 */
bool CFarmServer::ReqToBindInnerPort(farm::ReqToBindInnerPort_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqToBindInnerPort Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckToBindOuterPort( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType, 0);
		return false;
	}

	SubServerGroupPtr pSubSvrGroup = dynamic_cast<CSubServerGroup*>(pGroup.Get());
	if (!pSubSvrGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqToBindInnerPort Error!!, not convert group user id = %d\n", packet.senderId );
		m_Protocol.AckToBindOuterPort( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType, 0);
		return false;
	}

	const int bindPort = pSubSvrGroup->GetToBindInnerPort( CServerSessionAccess(GetServer()) );

	RemoteSubServerPtr pSubServer = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pSubServer)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqToBindInnerPort Error!!, not found user user id = %d\n", packet.senderId );
		m_Protocol.AckToBindOuterPort( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.bindSubServerSvrType, 0);
		return false;
	}

	pSubServer->SetInnerBindPort( packet.bindSubServerSvrType, bindPort );
	m_Protocol.AckToBindInnerPort( packet.senderId, SEND_T, error::ERR_SUCCESS, packet.bindSubServerSvrType, bindPort );
	return true;
}


/**
 @brief ReqSubServerBindComplete
 */
bool CFarmServer::ReqSubServerBindComplete(farm::ReqSubServerBindComplete_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqSubServerBindComplete Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckSubServerBindComplete( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType );
		return false;
	}

	SubServerGroupPtr pSubSvrGroup = dynamic_cast<CSubServerGroup*>(pGroup.Get());
	if (!pSubSvrGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "ReqSubServerBindComplete Error!!, not convert group user id = %d\n", packet.senderId );
		m_Protocol.AckSubServerBindComplete( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType );
		return false;
	}

	RemoteSubServerPtr pClient = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pClient)
	{
		clog::Error( log::ERROR_PROBLEM, "AckSubServerBindComplete Error!!, not exist user id = %d\n", packet.senderId );
		m_Protocol.AckSubServerBindComplete( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.bindSubServerSvrType );
		return false;
	}

	pClient->SetBindComplete(packet.bindSubServerSvrType);
	m_Protocol.AckSubServerBindComplete( packet.senderId, SEND_T, error::ERR_SUCCESS, packet.bindSubServerSvrType );

	if (packet.bindSubServerSvrType == "client")
		return false;

	// pClient 에 p2p link 가 있거나, input_link 가 있는 서버에게 메세지를 보낸다.
	// 다시 해석하면 pClient에게 p2pS link, output_link 인 서버에게 메세지를 보낸다.
	std::vector<network::SHostInfo> bindInfo;
	bindInfo.reserve(10);
	pClient->GetServerInfoCorrespondClientLink(packet.bindSubServerSvrType, bindInfo);
	if (bindInfo.empty())
	{
		clog::Error( clog::ERROR_PROBLEM, "Not Found Bind Server binSvrType : %s", packet.bindSubServerSvrType.c_str() );
		return false;
	}
	if (bindInfo.size() > 1)
	{
		clog::Error( clog::ERROR_CRITICAL, "Too Many Bind Server Found binSvrType : %s", packet.bindSubServerSvrType.c_str() );
		return false;
	}

	std::vector<std::string> links;
	pSubSvrGroup->GetCorrespondClientInfo( CServerSessionAccess(GetServer()), links );
	BOOST_FOREACH(auto &svrType, links)
	{
		SubServerGroupPtr pGroup = FindGroup(svrType);
		if (!pGroup)
			continue;

		clog::Log( clog::LOG_F_N_O, clog::LOG_MESSAGE, 0, "CorrespondClientInfo %s", svrType.c_str() );

		m_Protocol.BindSubServer( pGroup->GetNetId(), SEND_T, pSubSvrGroup->GetSvrType(), 
			bindInfo.front().ip, bindInfo.front().portnum );
	}
	return true;
}


/**
 @brief ReqSubClientConnectComplete
 */
bool CFarmServer::ReqSubClientConnectComplete(farm::ReqSubClientConnectComplete_Packet &packet)
{
	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (!pGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "AckSubClientConnectComplete Error!!, not exist group user id = %d\n", packet.senderId );
		m_Protocol.AckSubClientConnectComplete( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType );
		return false;
	}

	SubServerGroupPtr pSubSvrGroup = dynamic_cast<CSubServerGroup*>(pGroup.Get());
	if (!pSubSvrGroup)
	{// Error!!
		clog::Error( log::ERROR_PROBLEM, "AckSubClientConnectComplete Error!!, not convert group user id = %d\n", packet.senderId );
		m_Protocol.AckSubClientConnectComplete( packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP, packet.bindSubServerSvrType );
		return false;
	}

	RemoteSubServerPtr pClient = dynamic_cast<CRemoteSubServer*>(
		GetServer()->GetSession(packet.senderId));
	if (!pClient)
	{
		clog::Error( log::ERROR_PROBLEM, "AckSubClientConnectComplete Error!!, not exist user id = %d\n", packet.senderId );
		m_Protocol.AckSubClientConnectComplete( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.bindSubServerSvrType );
		return false;
	}

	pClient->SetConnectComplete(packet.bindSubServerSvrType);
	m_Protocol.AckSubClientConnectComplete( packet.senderId, SEND_T, error::ERR_SUCCESS, packet.bindSubServerSvrType );
	return true;
}

