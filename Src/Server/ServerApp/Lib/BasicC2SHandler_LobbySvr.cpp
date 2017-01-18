
#include "stdafx.h"
#include "BasicC2SHandler_LobbySvr.h"
#include "LobbyServer.h"
#include "../DataStructure/LobbyPlayer.h"


using namespace network;


CBasicC2SHandler_LobbySvr::CBasicC2SHandler_LobbySvr( multinetwork::CMultiPlug &certifySvr, CServerBasic &svr) : 
	CBasicC2SHandler(svr)
{
	svr.RegisterProtocol(&m_BasicProtocol);
	certifySvr.RegisterProtocol(&m_CertifyProtocol);
}

CBasicC2SHandler_LobbySvr::~CBasicC2SHandler_LobbySvr()
{
}


/**
 @brief ReqLogIn
 */
bool CBasicC2SHandler_LobbySvr::ReqLogIn( basic::ReqLogIn_Packet &packet)
{
	if (!CBasicC2SHandler::ReqLogIn(packet))
		return false;

	CSession *pSession = network::CheckClientNetId( &GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher );
	RETV(!pSession, false);

	MultiPlugDelegationPtr pLobbySvrDelegation = CheckDelegation( "client", packet.senderId, &m_BasicProtocol, packet.pdispatcher );
	RETV(!pLobbySvrDelegation, false);

	CLobbyServer *pLobbySvr = CheckCasting<CLobbyServer*>( pLobbySvrDelegation.Get(), packet.senderId, &m_BasicProtocol, packet.pdispatcher );
	RETV(!pLobbySvr, false);

	PlayerPtr pPlayer = GetServer().GetPlayer(packet.id);
	if (!pPlayer)
	{
		clog::Error( clog::ERROR_CRITICAL, 0, "ReqLogIn Error!! not found user id=%s", packet.id.c_str());
		m_BasicProtocol.AckLogIn( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.id, packet.c_key);
		return false;
	}

	// check certify key
	if (pPlayer->GetCertifyKey() != packet.c_key)
	{
		clog::Error( clog::ERROR_PROBLEM, 0, "ReqLogIn Error!! invalid certify key key=%d", packet.c_key );
		m_BasicProtocol.AckLogIn( packet.senderId, SEND_T, error::ERR_INVALID_CERTIFY_KEY, packet.id, packet.c_key);
		return false;
	}

	// player mapping
	PlayerPtr pAutoConstuctPlayer = GetServer().GetPlayer(packet.senderId);
	if (!pAutoConstuctPlayer)
	{
		clog::Error( clog::ERROR_CRITICAL, 0, "ReqLogIn Error!! not foudn auto construct player netid=%d", packet.senderId);
		m_BasicProtocol.AckLogIn( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.id, packet.c_key);

		GetServer().RemovePlayer(pPlayer->GetNetId());
		return false;
	}

	pSession->SetName(packet.id);
	pSession->SetCertifyKey(packet.c_key);
	pSession->SetState( SESSIONSTATE_LOGIN );

	// player mapping
	// *pAutoConstuctPlayer = *pPlayer
	pAutoConstuctPlayer->SetName( pPlayer->GetName() );
	// remove temperate player object
	GetServer().RemovePlayer(pPlayer->GetNetId());

	m_CertifyProtocol.ReqUserMoveServer( SERVER_NETID, SEND_T, packet.id, "lobbysvr" );
	m_BasicProtocol.AckLogIn( packet.senderId, SEND_T, error::ERR_SUCCESS, packet.id, packet.c_key);
	return true;
}


/**
 @brief AckUserId
 */
bool CBasicC2SHandler_LobbySvr::AckUserLogin(certify::AckUserLogin_Packet &packet)
{
	CSession *pClient = network::CheckClientId( &GetServer(), packet.id, 0, &m_BasicProtocol, packet.pdispatcher );
	RETV(!pClient, false);

	if (packet.errorCode != error::ERR_SUCCESS)
	{
		clog::Error( clog::ERROR_PROBLEM, "AckUserId Error!! client generate user id Error id=%s", 
			packet.id.c_str());
		return false;
	}

	pClient->SetState(SESSIONSTATE_LOGIN); // login state
	m_BasicProtocol.AckLogIn(pClient->GetNetId(), SEND_T, packet.errorCode, packet.id, 0);
	return true;
}


/**
 @brief ReqMoveToServer
 */
bool CBasicC2SHandler_LobbySvr::ReqMoveToServer(basic::ReqMoveToServer_Packet &packet)
{
	//if (packet.serverName == "gamesvr")
	//{
	//	MultiPlugPtr ptr =CheckMultiPlug("gamesvr", packet.senderId, &m_BasicProtocol, packet.pdispatcher);
	//	RETV(!ptr,false);
	//	ptr->RegisterProtocol( &m_ServerNetworkProtocol );
	//	//m_ServerNetworkProtocol.ReqMovePlayer( 
	//}
	return true;
}


/**
 @brief ReqGroupCreate
		From Client
 */
bool CBasicC2SHandler_LobbySvr::ReqGroupCreate(basic::ReqGroupCreate_Packet &packet)
{
	//if (!CBasicC2SHandler::ReqGroupCreate(packet))
	//	return false;
	CPlayer *pPlayer = CheckRecvablePlayer(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher);
	RETV(!pPlayer, false);

	CLobbyPlayer *pLobbyPlayer = CheckCasting<CLobbyPlayer*>(pPlayer, packet.senderId, &m_BasicProtocol, packet.pdispatcher);
	RETV(!pLobbyPlayer, false);

	GroupPtr pParentGroup, pFrom, pNewGroup;
	if (!CreateBlankGroup(packet.senderId, packet.parentGroupId, packet.groupName, pParentGroup, pFrom, pNewGroup))
		return false; /// Error!!!

	MultiPlugDelegationPtr pGameSvrDelegation = CheckDelegation("gamesvr", packet.senderId, 
		&m_BasicProtocol, packet.pdispatcher);
	if (!pGameSvrDelegation)
	{ /// Error!!
		GetServer().GetRootGroup().RemoveChild(pNewGroup->GetNetId());
		return false;
	}
	//RETV(!pGameSvrDelegation, false);

	CSubServerPlug *pSubSvrPlug = CheckCasting<CSubServerPlug*>(pGameSvrDelegation.Get(), packet.senderId, 
		&m_BasicProtocol, packet.pdispatcher);
	if (!pSubSvrPlug)
	{/// Error!!!
		GetServer().GetRootGroup().RemoveChild(pNewGroup->GetNetId());
		return false;
	}
	//RETV(!pSubSvrPlug, false);

	std::list<SSubServerInfo> subServers = pSubSvrPlug->GetSubServerInfo();
	subServers.sort();
	if (subServers.empty())
	{ /// Error!!
		clog::Error( clog::ERROR_CRITICAL, "ReqGroupCreate Error!! not found lobbysvr server" );
		m_BasicProtocol.AckGroupCreate( packet.senderId, SEND_T, error::ERR_GROUPCREATE_NOT_FOUND_GAMESVR,
			packet.senderId, 0, packet.parentGroupId, packet.groupName);
		return false;
	}

	// Request Game Server to Create Group
	pLobbyPlayer->SetRequestState( CLobbyPlayer::REQ_CREATE_GROUP );		

	SSubServerInfo targetSvr = subServers.front();
	pSubSvrPlug->RegisterProtocol( &m_SvrNetworkProtocol );
	m_SvrNetworkProtocol.ReqCreateGroup( targetSvr.serverId, SEND_T, packet.groupName, 
		pNewGroup->GetNetId(), packet.senderId );
	return true;
}


/**
 @brief AckCreateGroup
			From GameServer
 */
bool CBasicC2SHandler_LobbySvr::AckCreateGroup(server_network::AckCreateGroup_Packet &packet)
{
	if (packet.errorCode == error::ERR_SUCCESS)
	{
		GroupPtr pGroup = CheckGroup(&GetServer(), packet.groupId, packet.reqPlayerId, &m_BasicProtocol, packet.pdispatcher);
		RETV(!pGroup, false);

		PlayerPtr pPlayer = CheckRecvablePlayer(&GetServer(), packet.reqPlayerId, &m_BasicProtocol, packet.pdispatcher);
		RETV(!pPlayer, false);

		m_SvrNetworkProtocol.ReqMovePlayer(packet.senderId, SEND_T, pPlayer->GetName(), pPlayer->GetCertifyKey(),
			pGroup->GetNetId(), " ", 0 );
	}
	else
	{ /// Error!!
		GroupPtr pGroup = CheckGroup(&GetServer(), packet.groupId, 0, NULL, packet.pdispatcher);
		if (pGroup)
		{
			GetServer().GetRootGroup().RemoveChild(pGroup->GetNetId());
		}
		m_BasicProtocol.AckGroupCreate(packet.reqPlayerId, SEND_T, packet.errorCode,
			packet.reqPlayerId, 0, 0, packet.name);
	}
	return true;
}


/**
 @brief AckMovePlayer 
			from GameServer
 */
bool CBasicC2SHandler_LobbySvr::AckMovePlayer(server_network::AckMovePlayer_Packet &packet)
{
	CPlayer *pPlayer = CheckPlayerId(&GetServer(), packet.id, 0, &m_BasicProtocol, packet.pdispatcher);
	RETV(!pPlayer, false);

	CLobbyPlayer *pLobbyPlayer = CheckCasting<CLobbyPlayer*>(pPlayer, pPlayer->GetNetId(), &m_BasicProtocol, packet.pdispatcher);
	RETV(!pLobbyPlayer, false);

	if (error::ERR_SUCCESS != packet.errorCode)
	{ /// Error!!
		if (CLobbyPlayer::REQ_CREATE_GROUP == pLobbyPlayer->GetRequestState())
		{
			m_BasicProtocol.AckGroupCreate( pLobbyPlayer->GetNetId(), SEND_T, error::ERR_INTERNAL, 
				pLobbyPlayer->GetNetId(), 0, 0, " ");
		}
		else
		{
			m_BasicProtocol.AckGroupJoin( pLobbyPlayer->GetNetId(), SEND_T, packet.errorCode,
				pLobbyPlayer->GetNetId(), packet.groupId );
		}
		return false;
	}

	GroupPtr pToGroup = CheckGroup(&GetServer(), packet.groupId, pLobbyPlayer->GetNetId(), &m_BasicProtocol, packet.pdispatcher);
	RETV(!pToGroup, false);

	GroupPtr pFromGroup = GetServer().GetRootGroup().GetChildFromPlayer( pLobbyPlayer->GetNetId() );
	if (!pFromGroup)
	{ /// Error!!
		clog::Error( clog::ERROR_CRITICAL, 0, "AckMovePlayer Error!! not found from group player id= %d", pLobbyPlayer->GetNetId() );

		if (CLobbyPlayer::REQ_CREATE_GROUP == pLobbyPlayer->GetRequestState())
		{
			m_BasicProtocol.AckGroupCreate( pLobbyPlayer->GetNetId(), SEND_T, error::ERR_INTERNAL, 
				pLobbyPlayer->GetNetId(), 0, 0, " ");
		}
		else
		{
			m_BasicProtocol.AckGroupJoin( pLobbyPlayer->GetNetId(), SEND_T, error::ERR_INTERNAL, 
				pLobbyPlayer->GetNetId(), packet.groupId );
		}
		return false;
	}

	pFromGroup->RemovePlayer(pFromGroup->GetNetId(), pLobbyPlayer->GetNetId());
	pToGroup->AddPlayer(pToGroup->GetNetId(), pLobbyPlayer->GetNetId());
	const netid parentId = (pToGroup->GetParent())? pToGroup->GetParent()->GetNetId() : 0;
	pToGroup->AddViewer( parentId );

	// Send Success
	if (CLobbyPlayer::REQ_CREATE_GROUP == pLobbyPlayer->GetRequestState())
	{
		m_BasicProtocol.AckGroupCreate( pLobbyPlayer->GetNetId(), SEND_T, error::ERR_SUCCESS, 
			pLobbyPlayer->GetNetId(), pToGroup->GetNetId(), parentId, pToGroup->GetName() );
	}
	else
	{
		m_BasicProtocol.AckGroupJoin( pLobbyPlayer->GetNetId(), SEND_T, error::ERR_SUCCESS, 
			pLobbyPlayer->GetNetId(), packet.groupId );
	}

	// Request Complete
	pLobbyPlayer->SetRequestState(CLobbyPlayer::REQ_NONE);

	// Send Move to Server
	MultiPlugDelegationPtr pGamesSvr = CheckDelegation("gamesvr", pLobbyPlayer->GetNetId(), &m_BasicProtocol, packet.pdispatcher );
	RETV(!pGamesSvr, false);

	CSubServerPlug* pSubServerPlug = CheckCasting<CSubServerPlug*>(pGamesSvr.Get(), pLobbyPlayer->GetNetId(), 
		&m_BasicProtocol, packet.pdispatcher);
	RETV(!pSubServerPlug, false);

	SSubServerInfo subSeverInfo = pSubServerPlug->GetSubServerInfo(packet.senderId);
	if (INVALID_NETID == subSeverInfo.serverId)
	{
		m_BasicProtocol.Error( pLobbyPlayer->GetNetId(), SEND_T, error::ERR_MOVETOSERVER_NOT_FOUND_SERVER );
		return false;
	}

	m_BasicProtocol.AckMoveToServer( pLobbyPlayer->GetNetId(), SEND_T, error::ERR_SUCCESS, "gamesvr", 
		subSeverInfo.ip, subSeverInfo.portnum );

	return true;
}
