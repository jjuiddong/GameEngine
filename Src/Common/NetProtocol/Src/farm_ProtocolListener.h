//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\farm_ProtocolListener.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "farm_ProtocolData.h"

namespace farm {

using namespace network;
using namespace marshalling;
static const int s2c_Dispatcher_ID = 2000;

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
	virtual bool AckSubServerLogin(farm::AckSubServerLogin_Packet &packet) { return true; }
	virtual bool AckSendSubServerP2PCLink(farm::AckSendSubServerP2PCLink_Packet &packet) { return true; }
	virtual bool AckSendSubServerP2PSLink(farm::AckSendSubServerP2PSLink_Packet &packet) { return true; }
	virtual bool AckSendSubServerInputLink(farm::AckSendSubServerInputLink_Packet &packet) { return true; }
	virtual bool AckSendSubServerOutputLink(farm::AckSendSubServerOutputLink_Packet &packet) { return true; }
	virtual bool AckServerInfoList(farm::AckServerInfoList_Packet &packet) { return true; }
	virtual bool AckToBindOuterPort(farm::AckToBindOuterPort_Packet &packet) { return true; }
	virtual bool AckToBindInnerPort(farm::AckToBindInnerPort_Packet &packet) { return true; }
	virtual bool AckSubServerBindComplete(farm::AckSubServerBindComplete_Packet &packet) { return true; }
	virtual bool AckSubClientConnectComplete(farm::AckSubClientConnectComplete_Packet &packet) { return true; }
	virtual bool BindSubServer(farm::BindSubServer_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 2100;

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
	virtual bool ReqSubServerLogin(farm::ReqSubServerLogin_Packet &packet) { return true; }
	virtual bool SendSubServerP2PCLink(farm::SendSubServerP2PCLink_Packet &packet) { return true; }
	virtual bool SendSubServerP2PSLink(farm::SendSubServerP2PSLink_Packet &packet) { return true; }
	virtual bool SendSubServerInputLink(farm::SendSubServerInputLink_Packet &packet) { return true; }
	virtual bool SendSubServerOutputLink(farm::SendSubServerOutputLink_Packet &packet) { return true; }
	virtual bool ReqServerInfoList(farm::ReqServerInfoList_Packet &packet) { return true; }
	virtual bool ReqToBindOuterPort(farm::ReqToBindOuterPort_Packet &packet) { return true; }
	virtual bool ReqToBindInnerPort(farm::ReqToBindInnerPort_Packet &packet) { return true; }
	virtual bool ReqSubServerBindComplete(farm::ReqSubServerBindComplete_Packet &packet) { return true; }
	virtual bool ReqSubClientConnectComplete(farm::ReqSubClientConnectComplete_Packet &packet) { return true; }
};


}
