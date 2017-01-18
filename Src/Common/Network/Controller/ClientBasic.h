/**
Name:   ClientBasic.h
Author:  jjuiddong
Date:    3/17/2013

	CServer에 접속하는 Client를 표현한다.
	P2P 통신 기능
*/
#pragma once


namespace network
{
	class CP2PClient;
	class CCoreClient;
	class CClientBasic : public CPlug
	{

		friend class CNetLauncher;
		friend class CController;
		friend class CBasicS2CHandler;

	public:
		CClientBasic(PROCESS_TYPE procType);
		virtual ~CClientBasic();

		bool					Proc();
		bool					Stop();
		void					Disconnect();
		void					Close();

		bool					IsConnect() const;
		bool					IsP2PHostClient() const;
		CoreClientPtr	GetConnectSvrClient() const;

		// Overriding
		//virtual bool		AddProtocolListener(ProtocolListenerPtr pListener) override;
		//virtual bool		RemoveProtocolListener(ProtocolListenerPtr pListener) override;
		virtual bool		Send(netid netId, const SEND_FLAG flag, CPacket &packet) override;
		virtual bool		SendP2P(CPacket &packet);
		virtual bool		SendAll(CPacket &packet) override;

	private:
		// CoreClient/P2P Event Handler
		void		OnConnect(CNetEvent &event );
		void		OnDisconnect(CNetEvent &event);
		void		OnMemberJoin(CNetEvent &event);
		void		OnMemberLeave(CNetEvent &event);

		void		Clear();

	private:
		CCoreClient	*m_pConnectSvr;			// p2p Server와 연결되는 CoreClient
		CP2PClient	*m_pP2p;

	};


	inline CoreClientPtr CClientBasic::GetConnectSvrClient() const { return CoreClientPtr(m_pConnectSvr); }

}
