//------------------------------------------------------------------------
// Name:    TaskLogic.h
// Author:  jjuiddong
// Date:    12/22/2012
// 
// CPacketQueue에 저장된 패킷을 가져와 파싱해서 Listener에 
// 등록된 객체에게 메세지를 보낸다.
//
// 현재 서버에서 받는 패킷만 처리하고 있다. 클라이언트용 코드는 아직 없다.
//------------------------------------------------------------------------
#pragma once

#include "../Controller/Controller.h"
#include "../Service/AllProtocolListener.h"
#include "../ProtocolHandler/BasicProtocolDispatcher.h"

namespace network
{
	DECLARE_TYPE_NAME_SCOPE(network, CTaskLogic)
	class CTaskLogic : public common::CTask
		, public memmonitor::Monitor<CTaskLogic, TYPE_NAME(network::CTaskLogic)>
	{
	public:
		CTaskLogic();
		virtual ~CTaskLogic();
		virtual RUN_RESULT	Run() override;
		CPlug* GetReceiveConnector(netid netId, OUT CONNECTOR_TYPE &type);
		void UpdateSenderNetId(CPlug *pCon,  CONNECTOR_TYPE type, CPacket &packet);
	};


	inline CTaskLogic::CTaskLogic() : 
		CTask(1,"TaskLogic") 
	{
	}

	inline CTaskLogic::~CTaskLogic()
	{
	}


	/**
	 @brief 
	 */
	CPlug* CTaskLogic::GetReceiveConnector(netid netId, OUT CONNECTOR_TYPE &type)
	{
		CPlug *pCon = GetServer(netId);
		if (pCon) 
			type = CON_SERVER;
		if (!pCon) 
		{
			pCon = GetClient(netId);
			if (pCon) 
				type = CON_CLIENT;
		}
		if (!pCon) 
		{
			pCon = GetCoreClient(netId);
			if (pCon) type = CON_CORECLIENT;
		}
		return pCon;
	}


	/**
	 @brief 클라이언트에서 패킷을 받으면, packet 의 Sender가 서버 Id 로 설정한다.
	 */
	void CTaskLogic::UpdateSenderNetId(CPlug *pCon,  CONNECTOR_TYPE type, CPacket &packet)
	{
		switch (type)
		{
		case CON_SERVER: break; // nothing

		case CON_CORECLIENT:
			{
				CCoreClient *pClient = dynamic_cast<CCoreClient*>(pCon);
				if (pClient)
				{
					packet.SetSenderId(pClient->GetServerNetId());
				}				
			}
			break;
		case CON_CLIENT:
			{
				CClientBasic *pClient = dynamic_cast<CClientBasic*>(pCon);
				if (pClient && pClient->GetConnectSvrClient())
				{
					packet.SetSenderId( pClient->GetConnectSvrClient()->GetServerNetId() );
				}
			}
			break;
		}
	}


	/**
	 @brief Run
	 */
	inline common::CTask::RUN_RESULT CTaskLogic::Run()
	{
		// Main Timer
		CController::Get()->MainLoop();
		//

		CPacketQueue::SPacketData packetData;
		if (!CPacketQueue::Get()->PopPacket(packetData))
			return RR_CONTINUE;

		CONNECTOR_TYPE type;
		CPlug *pCon = GetReceiveConnector(packetData.rcvNetId, type);
		if (!pCon)
		{
			clog::Error( clog::ERROR_PROBLEM,
				common::format("CTaskLogic:: recv netid: %d, packet id: %d 에 해당하는 NetConnector가 없습니다.\n", 
				packetData.rcvNetId, packetData.packet.GetPacketId()) );
			return RR_CONTINUE;
		}

		UpdateSenderNetId(pCon, type, packetData.packet);

		const ProtocolListenerList &listeners = pCon->GetProtocolListeners();

		// 모든 패킷을 받아서 처리하는 리스너에게 패킷을 보낸다.
		all::Dispatcher allDispatcher;
		allDispatcher.Dispatch(packetData.packet, listeners);
		// 

		const int protocolId = packetData.packet.GetProtocolId();

		// 기본 프로토콜 처리
		if (protocolId == 0)
		{
			switch (type)
			{
			case CON_SERVER:
				{
					basic_protocol::ServerDispatcher dispatcher;
					dispatcher.Dispatch( packetData.packet, dynamic_cast<CServerBasic*>(pCon) );
				}
			case CON_CORECLIENT:
				{
					basic_protocol::ClientDispatcher dispatcher;
					dispatcher.Dispatch( packetData.packet, dynamic_cast<CCoreClient*>(pCon) );
				}
				break;
			}
			return RR_CONTINUE;
		}

		if (listeners.empty())
		{
			clog::Error( clog::ERROR_CRITICAL,
				common::format("CTaskLogic %d NetConnector의 protocolId = %d 프로토콜 리스너가 없습니다.\n", 
				pCon->GetNetId(), protocolId) );
			return RR_CONTINUE;
		}

		IProtocolDispatcher *pDispatcher = CController::Get()->GetDispatcher(protocolId);
		if (!pDispatcher)
		{
			clog::Error( clog::ERROR_PROBLEM,
				common::format("CTaskLogic:: %d 에 해당하는 프로토콜 디스패쳐가 없습니다.\n", 
				protocolId) );
			return RR_CONTINUE;
		}

		if (!pDispatcher->Dispatch(packetData.packet, listeners))
		{
			clog::Error( clog::ERROR_CRITICAL,
				common::format("CTaskLogic netid: %d  packet id: %d NetConnector의 프로토콜 리스너가 없습니다.\n", 
				pCon->GetNetId(), packetData.packet.GetPacketId()) );
		}

		return RR_CONTINUE;
	}
}
