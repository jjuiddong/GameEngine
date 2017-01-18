/**
Name:   RelayServer.h
Author:  jjuiddong
Date:    4/20/2013

 Relay Server
*/
#pragma once

DECLARE_TYPE_NAME(CRelayServer)
class CRelayServer : public network::CServer
	, public network::AllProtocolDisplayer
	, public memmonitor::Monitor<CRelayServer, TYPE_NAME(CRelayServer)>
{
public:
	CRelayServer();
	virtual ~CRelayServer();


};
