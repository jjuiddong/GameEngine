/**
Name:   RemoteClient.h
Author:  jjuiddong
Date:    12/25/2012

서버에 접속된 클라이언트 정보를 가진다.
*/
#pragma once

namespace network
{
	////DECLARE_TYPE_NAME_SCOPE(network, CRemoteClient)
	//class CRemoteClient 
	//	//: public memmonitor::Monitor<CRemoteClient, TYPE_NAME(network::CRemoteClient)>
	//{
	//public:
	//	CRemoteClient();
	//	virtual ~CRemoteClient();

	//	netid		GetId() const;
	//	const std::string& GetName();
	//	SOCKET	GetSocket() const;
	//	netid		GetGroupId() const;
	//	P2P_STATE GetP2PState() const;
	//	CLIENT_STATE GetState() const;
	//	const std::string& GetIp() const;
	//	void			SetNetId(netid id) ;
	//	void			SetName(const std::string &name);
	//	void			SetSocket(SOCKET sock);
	//	void			SetP2PState(P2P_STATE state);
	//	void			SetState(CLIENT_STATE state);
	//	void			SetIp(const std::string &ip);

	//private:
	//	CLIENT_STATE	m_State;
	//	P2P_STATE m_P2PState;
	//	netid		m_Id;
	//	SOCKET	m_Socket;
	//	std::string m_Name;
	//	std::string m_Passwd;
	//	std::string m_Ip;

	//};


	//inline netid		CRemoteClient::GetId() const { return m_Id; }
	//inline const std::string& CRemoteClient::GetName() { return m_Name; }
	//inline SOCKET	CRemoteClient::GetSocket() const { return m_Socket; }
	//inline P2P_STATE CRemoteClient::GetP2PState() const { return m_P2PState; }
	//inline CLIENT_STATE CRemoteClient::GetState() const { return m_State; }
	//inline const std::string& CRemoteClient::GetIp() const { return m_Ip; }
	//inline void			CRemoteClient::SetNetId(netid id) { m_Id = id; }
	//inline void			CRemoteClient::SetName(const std::string &name) { m_Name = name; }
	//inline void			CRemoteClient::SetSocket(SOCKET sock) { m_Socket = sock; }
	//inline void			CRemoteClient::SetP2PState(P2P_STATE state) { m_P2PState = state; }
	//inline void			CRemoteClient::SetState(CLIENT_STATE state) { m_State = state; }
	//inline void			CRemoteClient::SetIp(const std::string &ip) { m_Ip = ip; }

}
