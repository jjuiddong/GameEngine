//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\server_network_ProtocolListener.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "server_network_ProtocolData.h"

namespace server_network {

using namespace network;
using namespace marshalling;
static const int s2s_Dispatcher_ID = 800;

// Protocol Dispatcher
class s2s_Dispatcher: public network::IProtocolDispatcher
{
public:
	s2s_Dispatcher();
protected:
	virtual bool Dispatch(network::CPacket &packet, const ProtocolListenerList &listeners) override;
};


// ProtocolListener
class s2s_ProtocolListener : virtual public network::IProtocolListener
{
	friend class s2s_Dispatcher;
	virtual bool ReqMovePlayer(server_network::ReqMovePlayer_Packet &packet) { return true; }
	virtual bool AckMovePlayer(server_network::AckMovePlayer_Packet &packet) { return true; }
	virtual bool ReqMovePlayerCancel(server_network::ReqMovePlayerCancel_Packet &packet) { return true; }
	virtual bool AckMovePlayerCancel(server_network::AckMovePlayerCancel_Packet &packet) { return true; }
	virtual bool ReqCreateGroup(server_network::ReqCreateGroup_Packet &packet) { return true; }
	virtual bool AckCreateGroup(server_network::AckCreateGroup_Packet &packet) { return true; }
	virtual bool SendServerInfo(server_network::SendServerInfo_Packet &packet) { return true; }
};


}
