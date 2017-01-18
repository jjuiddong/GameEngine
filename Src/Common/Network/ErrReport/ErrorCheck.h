/**
Name:   ErrorCheck.h
Author:  jjuiddong
Date:    4/22/2013

	에러 체크를 처리하는 함수들 모음
*/
#pragma once

namespace basic { class s2c_Protocol; }

namespace network
{
	/// Session
	CSession* CheckSessionLogin(ServerBasicPtr pServer, netid clientId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CSession* CheckSessionIdLogin(ServerBasicPtr pServer, const std::string &id, netid clientId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CSession* CheckClientNetId( ServerBasicPtr pServer, netid clientId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CSession* CheckClientId( ServerBasicPtr pServer, const std::string &id, netid clientId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	GroupPtr CheckGroup( ServerBasicPtr pServer, netid groupId, netid clientId,
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	bool CheckClientConnection( CSession *pClient, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );


	/// Player
	CPlayer* CheckPlayerNetId(ServerBasicPtr pServer, netid playerId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CPlayer* CheckPlayerNetId_(ServerBasicPtr pServer, netid playerId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CPlayer* CheckPlayerId(ServerBasicPtr pServer, const std::string &id, netid playerId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CPlayer* CheckPlayerId_(ServerBasicPtr pServer, const std::string &id, netid playerId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CPlayer* CheckPlayerWaitAck(ServerBasicPtr pServer, netid clientId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );

	CPlayer* CheckRecvablePlayer(ServerBasicPtr pServer, netid clientId, 
		basic::s2c_Protocol *pProtocol, IProtocolDispatcher *pDispatcher );



	MultiPlugDelegationPtr CheckDelegation( const std::string &linkSvrType, 
		netid clientId=0, basic::s2c_Protocol *pProtocol=NULL, IProtocolDispatcher *pDispatcher=NULL );

	MultiPlugPtr CheckMultiPlug( const std::string &linkSvrType, 
		netid clientId=0, basic::s2c_Protocol *pProtocol=NULL, IProtocolDispatcher *pDispatcher=NULL );


	template<class T1, class T2>
	inline T1 CheckCasting( T2 ptr, netid clientId=0, basic::s2c_Protocol *pProtocol=NULL, IProtocolDispatcher *pDispatcher=NULL )
	{
		T1 p1 = dynamic_cast<T1>(ptr);
		if (!p1)
		{
			clog::Error( clog::ERROR_CRITICAL, 0, "Casting Error!!!" );
			if (pDispatcher)
				pDispatcher->PrintThisPacket( clog::LOG_FILE, "!!! Error casting err >>" );
			if (clientId && pProtocol)
				pProtocol->Error( clientId, SEND_T, error::ERR_INTERNAL );
		}
		return p1;
	}


}
