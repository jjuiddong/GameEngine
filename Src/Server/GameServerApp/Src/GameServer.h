/**
Name:   GameServer.h
Author:  jjuiddong
Date:    4/18/2013

게임 서버 
*/
#pragma once

#include "NetProtocol/src/server_network_Protocol.h"
#include "NetProtocol/Src/server_network_ProtocolListener.h"

DECLARE_TYPE_NAME(CGameServer)
class CGameServer : public network::CServer
								, public server_network::s2s_ProtocolListener

								, public network::AllProtocolDisplayer
								, public memmonitor::Monitor<CGameServer, TYPE_NAME(CGameServer)>
{
	enum
	{
		ID_TIMER_REFRESH = 100,
		REFRESH_TIMER_INTERVAL = 10000,
	};

public:
	CGameServer();
	virtual ~CGameServer();

protected:
	// Event Handler
	virtual void	OnConnectMultiPlug() override;
	void				OnTimer( network::CEvent &event );	


	// Network Protocol Handler
	virtual bool ReqMovePlayer(server_network::ReqMovePlayer_Packet &packet) override;
	virtual bool ReqMovePlayerCancel(server_network::ReqMovePlayerCancel_Packet &packet) override;
	virtual bool ReqCreateGroup(server_network::ReqCreateGroup_Packet &packet) override;
	virtual bool SendServerInfo(server_network::SendServerInfo_Packet &packet) override;
	
private:
	server_network::s2s_Protocol m_SvrNetworkProtocol;

};
