
#include "stdafx.h"
#include "GameServer.h"
#include "GamePlayer.h"

using namespace network;

CGameServer::CGameServer()
{

}

CGameServer::~CGameServer()
{

}


/**
@brief  OnConnectMultiPlug
*/
void	CGameServer::OnConnectMultiPlug()
{
	MultiPlugDelegationPtr pLobbySvrDelegation = CheckDelegation("lobbysvr");
	RET(!pLobbySvrDelegation);

	AddChild(pLobbySvrDelegation);

	AddProtocolListener(this);

	EVENT_CONNECT_TO( GetServer(), this, EVT_TIMER, CGameServer, CGameServer::OnTimer );
	GetServer()->AddTimer(ID_TIMER_REFRESH, REFRESH_TIMER_INTERVAL);

}


/**
@brief  OnTimer
*/
void	CGameServer::OnTimer( network::CEvent &event )
{
	if (ID_TIMER_REFRESH == event.GetParam())
	{
		if (GetServer() && GetServer()->IsServerOn())
		{
			MultiPlugPtr pLoginSvrController = multinetwork::CMultiNetwork::Get()->GetMultiPlug("lobbysvr");
			if (pLoginSvrController)
			{
				pLoginSvrController->RegisterProtocol( &m_SvrNetworkProtocol );
				m_SvrNetworkProtocol.SendServerInfo( ALL_NETID, network::SEND_T, "gamesvr", 
					GetServer()->GetIp(), GetServer()->GetPort(), GetServer()->GetSessions().size() );
			}
		}
	}

}


/**
 @brief ReqMovePlayer
 */
bool CGameServer::ReqMovePlayer(server_network::ReqMovePlayer_Packet &packet)
{
	CSession *pSession = CheckClientId(GetServer(), packet.id, 0, NULL, NULL);
	if (pSession) // Already exist
	{///!!Error
		clog::Error( clog::ERROR_PROBLEM, 0, "ReqMovePlayer Player already exist netid: %d, id=%s", 
			pSession->GetNetId(), pSession->GetName().c_str() );
		m_SvrNetworkProtocol.AckMovePlayer(packet.senderId, SEND_T, error::ERR_MOVEUSER_ALREADY_EXIST,
			packet.id, packet.groupId, packet.ip, packet.port);
		return false;
	}

	GroupPtr pGroup = CheckGroup(GetServer(), packet.groupId, 0, NULL, packet.pdispatcher);
	if (!pGroup)
	{
		m_SvrNetworkProtocol.AckMovePlayer(packet.senderId, SEND_T, error::ERR_NOT_FOUND_GROUP,
			packet.id, packet.groupId, packet.ip, packet.port);
		return false;
	}

	/// Add Player
	CPlayer *pPlayer = GetServer()->GetPlayerFactory()->New();
	pPlayer->SetName(packet.id);
	pPlayer->SetCertifyKey(packet.c_key);
	GetServer()->AddPlayer( pPlayer );

	if (!pGroup->AddPlayer( pGroup->GetNetId(), pPlayer->GetNetId() ))
	{
		clog::Error( clog::ERROR_PROBLEM, 0, "ReqMovePlayer player group join Error groupId: %d, playerId=%d", 
			packet.groupId, pPlayer->GetNetId() );
		m_SvrNetworkProtocol.AckMovePlayer(packet.senderId, SEND_T, error::ERR_NOT_JOIN_GROUP,
			packet.id, packet.groupId, packet.ip, packet.port);

		GetServer()->RemovePlayer( pPlayer->GetNetId() );
		return false;
	}

	m_SvrNetworkProtocol.AckMovePlayer(packet.senderId, SEND_T, error::ERR_SUCCESS, packet.id, packet.groupId, packet.ip, packet.port);
	return true;
}


/**
 @brief ReqMoveplayerCancel
 */
bool CGameServer::ReqMovePlayerCancel(server_network::ReqMovePlayerCancel_Packet &packet)
{

	return true;
}


/**
 @brief ReqCreateGroup
 */
bool CGameServer::ReqCreateGroup(server_network::ReqCreateGroup_Packet &packet)
{
	GroupPtr pGroup = CheckGroup(GetServer(), packet.groupId, 0, NULL, NULL);
	if (pGroup)
	{
		m_SvrNetworkProtocol.AckCreateGroup(packet.senderId, SEND_T, 
			error::ERR_GROUPJOIN_ALREADY_SAME_GROUP, packet.name, packet.groupId, packet.reqPlayerId );
		return false;
	}

	CGroup *pNewGroup = GetServer()->GetGroupFactory()->New();
	pNewGroup->SetNetId(packet.groupId);
	pNewGroup->SetName(packet.name);
	
	if (!GetServer()->GetRootGroup().AddChild(pNewGroup))
	{
		clog::Error( clog::ERROR_PROBLEM, 0, "ReqCreateGroup did'nt add child group groupId: %d", 
			packet.groupId);
		m_SvrNetworkProtocol.AckCreateGroup(packet.senderId, SEND_T, 
			error::ERR_NO_CREATE_GROUP, packet.name, packet.groupId, packet.reqPlayerId );

		SAFE_DELETE(pNewGroup);
		return false;
	}

	m_SvrNetworkProtocol.AckCreateGroup(packet.senderId, SEND_T, error::ERR_SUCCESS, 
		packet.name, packet.groupId, packet.reqPlayerId );
	return true;
}


/**
 @brief SendServerInfo
	receiv from lobby server
 */
bool CGameServer::SendServerInfo(server_network::SendServerInfo_Packet &packet)
{

	return true;
}

