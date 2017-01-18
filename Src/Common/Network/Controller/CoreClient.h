/**
Name:   CoreClient.h
Author:  jjuiddong
Date:    2/28/2013

CServer �� �����ϴ� Ŭ���̾�Ʈ Ŭ������. 
*/
#pragma once


namespace network
{

	class CCoreClient : public CPlug
	{
		friend class CNetLauncher;
		friend class CController;
		friend class CClientBasic;
		friend class CP2PClient;

	public:
		CCoreClient(PROCESS_TYPE procType);
		virtual ~CCoreClient();

		bool				Stop();
		void				Disconnect();
		void				Close();

		void				SetConnect(bool isConnect);
		netid			GetServerNetId() const;

		virtual bool	Send(netid netId, const SEND_FLAG flag, CPacket &packet);
		virtual bool	SendAll(CPacket &packet) override;

		// Event Handler
		void				OnConnect();
		void				OnDisconnect();
		void				OnMemberJoin(netid netId); /// P2P member join
		void				OnMemberLeave(netid netId); /// P2P member leave

	protected:
		bool				Proc();
		void				DispatchPacket();
		void				Clear();

	private:
		netid			m_ServerNetId;			// ������ NetId
	};


	inline void	 CCoreClient::SetConnect(bool isConnect) { SetState(isConnect? SESSIONSTATE_LOGIN : SESSIONSTATE_DISCONNECT); }
	inline netid CCoreClient::GetServerNetId() const { return m_ServerNetId; }

}
