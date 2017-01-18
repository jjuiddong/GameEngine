//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\certify_ProtocolListener.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "certify_ProtocolData.h"

namespace certify {

using namespace network;
using namespace marshalling;
static const int s2s_Dispatcher_ID = 900;

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
	virtual bool ReqUserLogin(certify::ReqUserLogin_Packet &packet) { return true; }
	virtual bool AckUserLogin(certify::AckUserLogin_Packet &packet) { return true; }
	virtual bool ReqUserMoveServer(certify::ReqUserMoveServer_Packet &packet) { return true; }
	virtual bool AckUserMoveServer(certify::AckUserMoveServer_Packet &packet) { return true; }
	virtual bool ReqUserLogout(certify::ReqUserLogout_Packet &packet) { return true; }
	virtual bool AckUserLogout(certify::AckUserLogout_Packet &packet) { return true; }
};


}
