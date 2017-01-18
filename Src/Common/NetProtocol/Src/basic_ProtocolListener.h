//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\basic_ProtocolListener.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "basic_ProtocolData.h"

namespace basic {

using namespace network;
using namespace marshalling;
static const int s2c_Dispatcher_ID = 100;

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
	virtual bool Error(basic::Error_Packet &packet) { return true; }
	virtual bool AckLogIn(basic::AckLogIn_Packet &packet) { return true; }
	virtual bool AckLogOut(basic::AckLogOut_Packet &packet) { return true; }
	virtual bool AckMoveToServer(basic::AckMoveToServer_Packet &packet) { return true; }
	virtual bool AckGroupList(basic::AckGroupList_Packet &packet) { return true; }
	virtual bool AckGroupJoin(basic::AckGroupJoin_Packet &packet) { return true; }
	virtual bool AckGroupCreate(basic::AckGroupCreate_Packet &packet) { return true; }
	virtual bool AckGroupCreateBlank(basic::AckGroupCreateBlank_Packet &packet) { return true; }
	virtual bool JoinMember(basic::JoinMember_Packet &packet) { return true; }
	virtual bool AckP2PConnect(basic::AckP2PConnect_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 200;

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
	virtual bool ReqLogIn(basic::ReqLogIn_Packet &packet) { return true; }
	virtual bool ReqLogOut(basic::ReqLogOut_Packet &packet) { return true; }
	virtual bool ReqMoveToServer(basic::ReqMoveToServer_Packet &packet) { return true; }
	virtual bool ReqGroupList(basic::ReqGroupList_Packet &packet) { return true; }
	virtual bool ReqGroupJoin(basic::ReqGroupJoin_Packet &packet) { return true; }
	virtual bool ReqGroupCreate(basic::ReqGroupCreate_Packet &packet) { return true; }
	virtual bool ReqGroupCreateBlank(basic::ReqGroupCreateBlank_Packet &packet) { return true; }
	virtual bool ReqP2PConnect(basic::ReqP2PConnect_Packet &packet) { return true; }
	virtual bool ReqP2PConnectTryResult(basic::ReqP2PConnectTryResult_Packet &packet) { return true; }
};


}
