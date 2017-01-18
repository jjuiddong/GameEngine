/**
Name:   ServerBasic.h
Author:  jjuiddong
Date:    2012-11-27

클라이언트가 접속할 Server를 표현한다.
*/
#pragma once

namespace network
{

	class CServerBasic : public CPlug
	{
		friend class CNetLauncher;
		friend class CController;
		friend class CTaskLogic;
		friend class CP2PClient;

	public:
		CServerBasic(PROCESS_TYPE procType);
		virtual ~CServerBasic();

		/// Packet
		virtual bool	Send(netid netId, const SEND_FLAG flag, CPacket &packet) override;
		virtual bool	SendAll(CPacket &packet) override;

		/// User
		bool				AddPlayer(CPlayer *pUser);
		bool				RemovePlayer(CPlayer *pUser);
		bool				RemovePlayer(netid netId);
		PlayerPtr		GetPlayer(netid netId);
		PlayerPtr		GetPlayer(const std::string &id);

		/// Session
		bool				AddSession(SOCKET sock, const std::string &ip);
		CSession*	GetSession(netid netId);
		CSession*	GetSession(const std::string &clientId);
		bool				RemoveSession(netid netId);
		bool				RemoveSessionSocket(netid netId);
		Sessions_V& GetSessions();
		bool				IsExist(netid netId);
		netid			GetNetIdFromSocket(SOCKET sock);

		/// Group
		CGroup&		GetRootGroup();

		/// Factory
		void				SetSessionFactory( ISessionFactory *ptr );
		ISessionFactory* GetSessionFactory() const;
		void				SetGroupFactory( IGroupFactory *ptr );
		IGroupFactory* GetGroupFactory() const;
		void				SetPlayerFactory( IPlayerFactory *ptr );
		IPlayerFactory* GetPlayerFactory() const;

		/// Etc
		bool				IsServerOn() const;
		void				Proc();
		bool				Stop();
		void				Disconnect();
		void				Close();
		void				Clear();

		void				SetOption(bool IsLoginCheck);
		common::CriticalSection& GetCS();
		void				MakeFDSET( SFd_Set *pfdset);
		void				AddTimer( int id, int intervalTime, bool isRepeat = true );
		void				KillTimer( int id );

		// Event Handler
		void				OnListen();
		void				OnDisconnect();
		void				OnClientJoin(netid netId);
		void				OnClientLeave(netid netId);
		virtual void	OnTimer(int id);

	protected:
		/// Etc
		void				MainLoop();
		void				InitRootGroup();
		bool				AcceptProcess();
		void				DispatchPacket();
		bool				SendGroup(GroupPtr pGroup, CPacket &packet);
		bool				SendViewer(netid groupId, const SEND_FLAG flag, CPacket &packet);
		bool				SendViewerRecursive(netid viewerId, const netid exceptGroupId, CPacket &packet);

		bool				RemoveClientProcess();
		SessionItor	FindSessionBySocket(SOCKET sock);

	private:
		Sessions_						m_Sessions;					// 서버와 연결된 클라이언트 정보리스트
		Players_							m_Users;
		IPlayerFactory				*m_pPlayerFactory;
		ISessionFactory			*m_pSessionFactory;
		IGroupFactory			    *m_pGroupFactory;
		common::CriticalSection  m_CS;
		std::vector<STimer>	m_Timers;
		bool								m_IsLoginCheck;			// true 이면 로그인된 클라이언트에게만 패킷을 전송할 수 있다.

		CGroup							m_RootGroup;
		netid							m_WaitGroupId;			// waiting place before join concrete group
	};


	inline bool CServerBasic::IsServerOn() const { return GetState() == SESSIONSTATE_LOGIN; }
	inline CGroup&	CServerBasic::GetRootGroup() { return m_RootGroup; }
	inline common::CriticalSection& CServerBasic::GetCS() { return m_CS; }
	inline Sessions_V& CServerBasic::GetSessions() { return m_Sessions.m_Seq; }
	inline ISessionFactory* CServerBasic::GetSessionFactory() const { return m_pSessionFactory; }
	inline IGroupFactory* CServerBasic::GetGroupFactory() const { return m_pGroupFactory; }
	inline void	CServerBasic::SetPlayerFactory( IPlayerFactory *ptr ) { m_pPlayerFactory = ptr; }
	inline IPlayerFactory* CServerBasic::GetPlayerFactory() const { return m_pPlayerFactory; }
	inline void CServerBasic::SetOption(bool IsLoginCheck) { m_IsLoginCheck = IsLoginCheck; }

};
