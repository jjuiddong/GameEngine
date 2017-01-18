/**
Name:   P2PClient.h
Author:  jjuiddong
Date:    2/28/2013

P2P 통신을 하는 클래스다.

 발생시키는 이벤트
	- EVT_CONNECT
	- EVT_DISCONNECT
	- EVT_MEMBER_JOIN
	- EVT_MEMBER_LEAVE

*/
#pragma once

namespace network
{
	class CServerBasic;
	class CCoreClient;

	DECLARE_TYPE_NAME_SCOPE(network, CP2PClient)
	class CP2PClient : public CPlug
								, public memmonitor::Monitor<CP2PClient, TYPE_NAME(CP2PClient)>
	{
		friend class CController;
		friend class CClient;

	public:
		CP2PClient(PROCESS_TYPE procType);
		virtual ~CP2PClient();

		bool				Bind( const int port );
		bool				Connect( const std::string &ip, const int port  );
		bool				Proc();
		bool				Stop();
		void				Disconnect();
		void				Close();

		bool				IsConnect() const;
		bool				IsHostClient() const;

		// Overriding
		//virtual bool	AddProtocolListener(ProtocolListenerPtr pListener) override;
		//virtual bool	RemoveProtocolListener(ProtocolListenerPtr pListener) override;

		// Child Implementes
		virtual bool	Send(netid netId, const SEND_FLAG flag, CPacket &packet) override;
		virtual bool	SendAll(CPacket &packet) override;

	protected:
		void				Clear();
		bool				CreateP2PHost(const int port);
		bool				CreateP2PClient(const std::string &ip, const int port);

		// P2P Client Event Handler
		void				OnConnect(CNetEvent &event);
		void				OnDisconnect(CNetEvent &event);

		// P2P Host Event Handler;
		void				OnListen(CNetEvent &event);
		void				OnClientJoin(CNetEvent &event);
		void				OnClientLeave(CNetEvent &event);

	protected:
		P2P_STATE		m_State;
		CCoreClient		*m_pP2pClient;
		CServerBasic	*m_pP2pHost;
	};

	inline bool CP2PClient::IsConnect() const { return true; }
	inline bool CP2PClient::IsHostClient() const { return m_State == P2P_HOST; }

}
