/**
Name:   SubServerPlug.h
Author:  jjuiddong
Date:    4/25/2013

	서버가 다른 서브 서버들간에 통신을 하는 객체.
*/
#pragma once

#include "NetProtocol/Src/server_network_Protocol.h"
#include "NetProtocol/Src/server_network_ProtocolListener.h"


namespace network
{
	class CSubServerPlug : public multinetwork::CMultiPlugDelegation
										, public server_network::s2s_ProtocolListener
	{
	public:
		CSubServerPlug(const std::string &appSvrType);
		virtual ~CSubServerPlug();

		std::list<SSubServerInfo> GetSubServerInfo();
		SSubServerInfo GetSubServerInfo(netid serverId);

	protected:
		// Event
		virtual void	OnConnectMultiPlug() override;
		void				OnConnectSubLink(CNetEvent &event );

		/// Network Protocol Handler
		virtual bool SendServerInfo(server_network::SendServerInfo_Packet &packet) override;

	private:
		typedef common::StableVectorMap<netid, CRemoteServer*>	RemoteServers;
		std::string m_AppSvrType;
		RemoteServers m_RemoteServers;


		server_network::s2s_Protocol		m_ServerNetwork_Protocol;

	};

}
