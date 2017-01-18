
#include "stdafx.h"
#include "ErrorCheck.h"

#include "NetProtocol/Src/basic_Protocol.h"

using namespace network;


/**
 @brief ClientCheck
	clientId에 해당하는 클라이언트가 없다면, 에러 메세지를 클라이언트에게 보내고 NULL을 리턴한다.
 */
CSession* network::CheckClientNetId( ServerBasicPtr pServer, netid clientId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);

	CSession *pClient = pServer->GetSession(clientId);
	if (!pClient)
	{
		//clog::Error( clog::ERROR_PROBLEM, "Client Check Error!! client not found id=%d", clientId);
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error client not found from netid>>" );
		if (pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_NOT_FOUND_USER );
		return NULL;
	}
	return pClient;
}


/**
 @brief ClientCheck
  id 에 해당하는 클라이언트가 없다면, 에러 메세지를 클라이언트에게 보내고 NULL을 리턴한다.
 */
CSession* network::CheckClientId( ServerBasicPtr pServer, const std::string &id, netid clientId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);

	CSession *pClient = pServer->GetSession(id);
	if (!pClient)
	{
		//clog::Error( clog::ERROR_PROBLEM, "Client Check Error!! client not found id=%s", id.c_str());
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error client not found from id>>" );
		if (pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_NOT_FOUND_USER );
		return NULL;
	}
	return pClient;
}


/**
 @brief 
 */
GroupPtr network::CheckGroup( ServerBasicPtr pServer, netid groupId, 
	netid clientId, basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);

	GroupPtr pGroup = pServer->GetRootGroup().GetChild( groupId );
	if (!pGroup)
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error not found group >>" );
		if (pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_NOT_FOUND_GROUP);
		return NULL;
	}
	return pGroup;
}


/**
 @brief CheckClientConnection
 */
bool network::CheckClientConnection( CSession *pClient, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pClient, false);
	if (pClient->GetState() == SESSIONSTATE_LOGIN)
		return true;
	if (pDispatcher)
		pDispatcher->PrintThisPacket( clog::LOG_F, "!!! Error client not connection >>" );
	return false;
}


/**
@brief  CheckPlayerWaitAck
			Check Player IsAckWait State
*/
CPlayer* network::CheckPlayerWaitAck(ServerBasicPtr pServer, netid clientId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);
	PlayerPtr pPlayer = CheckPlayerNetId_(pServer, clientId, pProtocol, pDispatcher);
	RETV(!pPlayer, NULL);

	if (pPlayer->IsAckWait())
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error Player Wait Ack err >>" );
		if (clientId && pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_WAIT_ACK_PLAYER);
		return NULL;
	}

	return pPlayer;
}


/**
@brief  CheckSessionLogin
			Check Session state  SESSIONSTATE_LOGIN
*/
CSession* network::CheckSessionLogin( ServerBasicPtr pServer, netid clientId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);

	SessionPtr pClient = pServer->GetSession(clientId);
	if (!pClient)
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "CheckSessionLogin!!! Error not found Session >>" );
		if (clientId && pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_NOT_FOUND_USER);
		return NULL;
	}

	if (pClient->GetState() != SESSIONSTATE_LOGIN)
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "CheckSessionLogin!!! Error Session Not Login >>" );
		if (clientId && pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_NOT_FOUND_USER);
		return NULL;
	}
	return pClient;	
}


CSession* network::CheckSessionIdLogin(ServerBasicPtr pServer, const std::string &id, netid clientId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);

	SessionPtr pClient = pServer->GetSession(id);
	if (!pClient)
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "CheckSessionLogin!!! Error not found Session >>" );
		if (clientId && pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_NOT_FOUND_USER);
		return NULL;
	}

	if (pClient->GetState() != SESSIONSTATE_LOGIN)
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "CheckSessionLogin!!! Error Session Not Login >>" );
		if (clientId && pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_NOT_FOUND_USER);
		return NULL;
	}
	return pClient;	
}


/**
@brief  CheckPlayerWaitAck() && CheckSessionLogin()
*/
CPlayer* network::CheckRecvablePlayer(ServerBasicPtr pServer, netid clientId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	PlayerPtr pPlayer = CheckPlayerWaitAck(pServer, clientId, pProtocol, pDispatcher);
	RETV(!pPlayer,NULL);
	SessionPtr pSession = CheckSessionLogin(pServer, clientId, pProtocol, pDispatcher);
	RETV(!pSession, NULL);

	return pPlayer;
}


/**
 @brief CheckPlayerNetId
 */
CPlayer* network::CheckPlayerNetId( ServerBasicPtr pServer, netid playerId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);
	if (!CheckSessionLogin(pServer, playerId, pProtocol, pDispatcher))
		return NULL;
	PlayerPtr pPlayer = CheckPlayerNetId_(pServer, playerId, pProtocol, pDispatcher);
	RETV(!pPlayer,NULL);
	return pPlayer;
}


/**
 @brief CheckPlayerNetId_
 */
CPlayer* network::CheckPlayerNetId_(ServerBasicPtr pServer, netid playerId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);
	PlayerPtr pPlayer = pServer->GetPlayer(playerId);
	if (!pPlayer)
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error Player Wait Ack err >>" );
		if (playerId && pProtocol)
			pProtocol->Error( playerId, SEND_T, error::ERR_NOT_FOUND_USER);
		return NULL;
	}
	return pPlayer;
}


/**
 @brief CheckPlayerId
 */
CPlayer* network::CheckPlayerId(ServerBasicPtr pServer, const std::string &id, netid playerId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	if (!CheckSessionIdLogin(pServer, id, playerId, pProtocol, pDispatcher))
		return NULL;
	CPlayer *pPlayer = CheckPlayerId_(pServer, id, playerId, pProtocol, pDispatcher);
	return pPlayer;
}


/**
 @brief CheckPlayerId_
 */
CPlayer* network::CheckPlayerId_(ServerBasicPtr pServer, const std::string &id, netid playerId, 
	basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	RETV(!pServer, NULL);

	PlayerPtr pPlayer = pServer->GetPlayer(id);
	if (!pPlayer)
	{
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error player not found from id>>" );
		if (playerId && pProtocol)
			pProtocol->Error( playerId, SEND_T, error::ERR_NOT_FOUND_USER );
		return NULL;
	}
	return pPlayer;
}


/**
 @brief CheckDelegation
 */
MultiPlugDelegationPtr network::CheckDelegation( const std::string &linkSvrType,
	netid clientId, basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher )
{
	MultiPlugDelegationPtr ptr = multinetwork::CMultiNetwork::Get()->GetDelegation(linkSvrType);
	if (!ptr)
	{
		clog::Error( clog::ERROR_CRITICAL, 0, "CheckDelegation Error!!! linkSvrType=%s", linkSvrType.c_str() );
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error CheckDelegation >>" );
		if (clientId && pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_INTERNAL );
	}
	return ptr;
}


/**
 @brief 
 */
MultiPlugPtr network::CheckMultiPlug( const std::string &linkSvrType, 
	netid clientId, basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher)
{
	MultiPlugPtr ptr = multinetwork::CMultiNetwork::Get()->GetMultiPlug(linkSvrType);
	if (!ptr)
	{
		clog::Error( clog::ERROR_CRITICAL, 0, "CheckMultiPlug Error!!! linkSvrType=%s", linkSvrType.c_str() );
		if (pDispatcher)
			pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error CheckMultiPlug  >>" );
		if (clientId && pProtocol)
			pProtocol->Error( clientId, SEND_T, error::ERR_INTERNAL );
	}
	return ptr;
}

