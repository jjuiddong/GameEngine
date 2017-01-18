//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\login_ProtocolListener.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "login_ProtocolData.h"

namespace login {

using namespace network;
using namespace marshalling;
static const int s2c_Dispatcher_ID = 600;

// Protocol Dispatcher
class s2c_Dispatcher: public network::IProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(network::CPacket &packet, const ProtocolListenerList &listeners) override;
};


// ProtocolListener
class s2c_ProtocolListener : virtual public network::IProtocolListener
{
	friend class s2c_Dispatcher;
	virtual bool AckLobbyIn(login::AckLobbyIn_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 700;

// Protocol Dispatcher
class c2s_Dispatcher: public network::IProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(network::CPacket &packet, const ProtocolListenerList &listeners) override;
};


// ProtocolListener
class c2s_ProtocolListener : virtual public network::IProtocolListener
{
	friend class c2s_Dispatcher;
	virtual bool ReqLobbyIn(login::ReqLobbyIn_Packet &packet) { return true; }
};


}
