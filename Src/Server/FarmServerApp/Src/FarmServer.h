/**
Name:   FarmServer.h
현Author:  jjuiddong
Date:    4/7/2013

 팜 서버 구현 
*/
#pragma once

#include "NetProtocol\Src\farm_Protocol.h"
#include "NetProtocol\Src\farm_ProtocolListener.h"

DECLARE_TYPE_NAME(CFarmServer)
class CFarmServer : public network::CServer
								,public farm::c2s_ProtocolListener

								,public network::AllProtocolDisplayer
								,public memmonitor::Monitor<CFarmServer, TYPE_NAME(CFarmServer)>
{
public:
	CFarmServer();
	virtual ~CFarmServer();

	SubServerGroupPtr FindGroup(const std::string &svrType);

private:
	// Event Handling
	virtual void	OnConnectMultiPlug() override;

	// Network Packet Handling
	virtual bool ReqSubServerLogin(farm::ReqSubServerLogin_Packet &packet) override;
	virtual bool SendSubServerP2PCLink(farm::SendSubServerP2PCLink_Packet &packet) override;
	virtual bool SendSubServerP2PSLink(farm::SendSubServerP2PSLink_Packet &packet) override;
	virtual bool SendSubServerInputLink(farm::SendSubServerInputLink_Packet &packet) override;
	virtual bool SendSubServerOutputLink(farm::SendSubServerOutputLink_Packet &packet) override;
	virtual bool ReqServerInfoList(farm::ReqServerInfoList_Packet &packet) override;
	virtual bool ReqToBindOuterPort(farm::ReqToBindOuterPort_Packet &packet) override;
	virtual bool ReqToBindInnerPort(farm::ReqToBindInnerPort_Packet &packet) override;
	virtual bool ReqSubServerBindComplete(farm::ReqSubServerBindComplete_Packet &packet) override;
	virtual bool ReqSubClientConnectComplete(farm::ReqSubClientConnectComplete_Packet &packet) override;

private:
	farm::s2c_Protocol m_Protocol;
	ServerBasicPtr m_pServer;

};
