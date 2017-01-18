/**
Name:   BasicS2CHandler.h
Author:  jjuiddong
Date:    3/17/2013

	CBasicS2CHandler
*/
#pragma once

#include "NetProtocol/Src/basic_ProtocolListener.h"
#include "NetProtocol/Src/basic_Protocol.h"

namespace network
{
	class CClient;
	class CBasicS2CHandler : public common::CEventHandler
											, public basic::s2c_ProtocolListener
	{
		/// Move To Server State
		enum MOVE_STATE
		{
			CLIENT_END_MOVE,
			CLIENT_BEGIN_MOVE,
			CLIENT_CLOSE,
			CLIENT_CONNECT,
		};

	public:
		CBasicS2CHandler( CClient &svr );
		virtual ~CBasicS2CHandler();

	protected:
		void		OnConnectClient(CNetEvent &event);
		void		OnDisconnectClient(CNetEvent &event);

		// Network Handler
		virtual bool AckLogIn(basic::AckLogIn_Packet &packet) override;
		virtual bool AckP2PConnect(basic::AckP2PConnect_Packet &packet) override;
		virtual bool AckMoveToServer(basic::AckMoveToServer_Packet &packet) override;

	private:
		CClient							&m_Client;		/// CClient Reference 
		basic::c2s_Protocol		m_BasicProtocol;

		MOVE_STATE	m_ClientState;
		bool  m_IsMoveToServer;
	};

}
