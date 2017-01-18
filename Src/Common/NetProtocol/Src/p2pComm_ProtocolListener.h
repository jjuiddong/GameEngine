//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\p2pComm_ProtocolListener.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "p2pComm_ProtocolData.h"

namespace p2pComm {

using namespace network;
using namespace marshalling;
static const int c2c_Dispatcher_ID = 1000;

// Protocol Dispatcher
class c2c_Dispatcher: public network::IProtocolDispatcher
{
public:
	c2c_Dispatcher();
protected:
	virtual bool Dispatch(network::CPacket &packet, const ProtocolListenerList &listeners) override;
};


// ProtocolListener
class c2c_ProtocolListener : virtual public network::IProtocolListener
{
	friend class c2c_Dispatcher;
	virtual bool SendData(p2pComm::SendData_Packet &packet) { return true; }
};


}
