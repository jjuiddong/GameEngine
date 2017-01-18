/**
Name:   Session.h
Author:  jjuiddong
Date:    4/26/2013

 Session 
*/
#pragma once


namespace network
{
	
	class CSession
	{
	public:
		CSession();
		virtual ~CSession();

		netid		GetNetId() const;
		certify_key GetCertifyKey() const;
		SOCKET	GetSocket() const;
		bool			IsConnect() const;
		bool			IsLogin() const;
		P2P_STATE GetP2PState() const;
		SESSION_STATE GetState() const;
		const std::string& GetName() const;
		const std::string& GetIp() const;
		int			GetPort() const;

		void			SetNetId(netid id);
		void			SetCertifyKey(certify_key key);
		void			SetName(const std::string &name);
		void			SetSocket(SOCKET sock);
		void			SetP2PState(P2P_STATE state);
		void			SetState(SESSION_STATE state);
		void			SetIp(const std::string &ip);
		void			SetPort(int port);

		void			ClearConnection();

	private:
		netid		m_Id;
		certify_key m_CertifyKey;
		SOCKET	m_Socket;

		SESSION_STATE	m_State;
		P2P_STATE m_P2PState;
		std::string m_Name;
		std::string m_Ip;
		int m_Port;

	};


	inline netid		CSession::GetNetId() const { return m_Id; }
	inline certify_key CSession::GetCertifyKey() const { return m_CertifyKey; }
	inline const std::string& CSession::GetName() const { return m_Name; }
	inline SOCKET	CSession::GetSocket() const { return m_Socket; }
	inline P2P_STATE CSession::GetP2PState() const { return m_P2PState; }
	inline SESSION_STATE CSession::GetState() const { return m_State; }
	inline const std::string& CSession::GetIp() const { return m_Ip; }
	inline int			CSession::GetPort() const { return m_Port; }
	inline bool		CSession::IsLogin() const { return m_State ==  SESSIONSTATE_LOGIN; }

	inline void			CSession::SetNetId(netid id) { m_Id = id; }
	inline void			CSession::SetCertifyKey(certify_key key) { m_CertifyKey = key; }
	inline void			CSession::SetName(const std::string &name) { m_Name = name; }
	inline void			CSession::SetSocket(SOCKET sock) { m_Socket = sock; }
	inline void			CSession::SetP2PState(P2P_STATE state) { m_P2PState = state; }
	inline void			CSession::SetState(SESSION_STATE state) { m_State = state; }
	inline void			CSession::SetIp(const std::string &ip) { m_Ip = ip; }
	inline void			CSession::SetPort(int port) { m_Port = port; }

}
