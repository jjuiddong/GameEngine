/**
Name:   ServerConnector.h
Author:  jjuiddong
Date:    4/3/2013

FarmServer 에 접속해서, 필요한 정보를 요청하고, 응답을 받는다.

- FarmServer Login 
- p2p, input line, output link 정보를 farm server에 보낸다.
- bind 될 port 값, connect 할 서버의 ip, port 정보를 얻는다.
*/
#pragma once

#include "MultiNetworkUtility.h"
#include "MultiPlugDelegation.h"
#include "NetProtocol/Src/farm_Protocol.h"
#include "NetProtocol/Src/farm_ProtocolListener.h"

namespace network { namespace multinetwork {

	struct SSvrConfigData;
	class CFarmServerPlug : public CMultiPlugDelegation
										,public farm::s2c_ProtocolListener
										,public AllProtocolDisplayer
	{
	public:
		CFarmServerPlug( const std::string &svrType, const SSvrConfigData &config );
		virtual ~CFarmServerPlug() {}

		bool		Start( const std::string &ip, const int port );

	private:
		void		CreateLink();
		void		ConnectLink();
		bool		CreateSubController( SERVICE_TYPE serviceType, bool IsInnerBind,
			const std::string &connectSubSvrType, const std::string &bindSubSvrType );
		void		ConnectSubController(SERVICE_TYPE serviceType, bool IsInnerBind,
			const std::string &connectSubSvrType, const std::string &bindSubSvrType );

		// Event Handling
		virtual void	OnConnectMultiPlug() override;

		// network event handler
		void OnConnect(CNetEvent &event);
		void OnConnectLink(CNetEvent &event);
		void OnDisconnectLink(CNetEvent &event);

		// network packet handler
		virtual bool AckSubServerLogin(farm::AckSubServerLogin_Packet &packet) override;
		virtual bool AckSendSubServerP2PCLink(farm::AckSendSubServerP2PCLink_Packet &packet) override;
		virtual bool AckSendSubServerP2PSLink(farm::AckSendSubServerP2PSLink_Packet &packet) override;
		virtual bool AckSendSubServerInputLink(farm::AckSendSubServerInputLink_Packet &packet) override;
		virtual bool AckSendSubServerOutputLink(farm::AckSendSubServerOutputLink_Packet &packet) override;
		virtual bool AckServerInfoList(farm::AckServerInfoList_Packet &packet) override;
		virtual bool AckToBindOuterPort(farm::AckToBindOuterPort_Packet &packet) override;
		virtual bool AckToBindInnerPort(farm::AckToBindInnerPort_Packet &packet) override;
		virtual bool AckSubServerBindComplete(farm::AckSubServerBindComplete_Packet &packet) override;
		virtual bool AckSubClientConnectComplete(farm::AckSubClientConnectComplete_Packet &packet) override;
		virtual bool BindSubServer(farm::BindSubServer_Packet &packet) override;

	private:
		farm::c2s_Protocol m_Protocol;
		SSvrConfigData m_Config;
		bool	m_IsDetectedSendConfig;

	};

}}
