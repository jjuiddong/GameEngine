/**
Name:   NetGroupController.h
Author:  jjuiddong
Date:    4/1/2013

하나 이상의 서버군들에게 접속하는 클래스다.
	Server/Clients/P2p 를 관리한다.

	Propagate NetEvent
		- EVT_LISTEN (server)
		- EVT_CONNECT (client)
		- EVT_DISCONNECT
		- EVT_CLIENT_JOIN
		- EVT_CLIENT_LEAVE
		
*/
#pragma once

namespace network { namespace multinetwork {

	///  네트워크 그룹에 접근하는 Server/Client/P2p 를 관리하는 클래스.
	DECLARE_TYPE_NAME_SCOPE(network::multinetwork, CMultiPlug)
	class CMultiPlug : public CPlug
							, public memmonitor::Monitor<CMultiPlug, TYPE_NAME(network::multinetwork::CMultiPlug)>
	{
	public:
		enum STATE {
			WAIT,
			TRYCONNECT, // try connect
			RUN, // after connect
			END, // after call Stop()
		};

		CMultiPlug( SERVICE_TYPE type, const std::string &svrType, 
			const std::string &connectSvrType );
		virtual ~CMultiPlug();

		bool				Start(const std::string &ip, const int port);
		bool				Start(const std::vector<SHostInfo> &v);
		void				Stop();

		virtual bool	Send(netid netId, const SEND_FLAG flag, CPacket &packet) override;
		virtual bool	SendAll(CPacket &packet) override;

		const std::string& GetSvrType() const;
		const std::string& GetConnectSvrType() const;
		void				SetSessionFactory( ISessionFactory *ptr );
		void				SetGroupFactory( IGroupFactory *ptr );
		SERVICE_TYPE GetServiceType() const;
		bool				IsConnect() const;
		bool				IsTryConnect() const;
		void				SetTryConnect();
		CServerBasic* GetServer();
		const CoreClients_V& GetClients();
		SessionPtr GetSession(netid netId);
		CoreClientPtr GetClient(netid netId);
		CoreClientPtr GetClientFromServerNetId(netid serverNetId);


	protected:
		bool				Connect( SERVICE_TYPE type, const std::string &ip, const int port );


	private:
		// Event Handler
		void				OnConnect( CNetEvent &event );
		void				OnDisconnect( CNetEvent &event );


	private:
		STATE m_State;
		SERVICE_TYPE m_ServiceType;
		std::string m_svrType;
		std::string m_connectSvrType;
		std::string m_Ip;
		int m_Port;

		CServerBasic *m_pServer;
		CP2PClient *m_pP2p;
		CoreClients_ m_Clients;
		CoreClients_ m_RemoveClients;
		ISessionFactory *m_pSessionFactory;
		IGroupFactory *m_pGroupFactory;
		
	};


	inline const std::string& CMultiPlug::GetSvrType() const { return m_svrType; }
	inline const std::string& CMultiPlug::GetConnectSvrType() const { return m_connectSvrType; }
	inline SERVICE_TYPE CMultiPlug::GetServiceType() const { return m_ServiceType; }
	inline CServerBasic* CMultiPlug::GetServer() { return m_pServer; }
	inline const CoreClients_V& CMultiPlug::GetClients() { return m_Clients.m_Seq; }
	inline bool CMultiPlug::IsConnect() const { return (m_State == RUN) || (m_State == TRYCONNECT); }
	inline bool CMultiPlug::IsTryConnect() const { return (m_State == TRYCONNECT); }
	inline void	CMultiPlug::SetTryConnect() { m_State = TRYCONNECT; }

}}
