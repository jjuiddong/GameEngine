//------------------------------------------------------------------------
// Name:    TaskLogic.h
// Author:  jjuiddong
// Date:    12/22/2012
// 
// CPacketQueue�� ����� ��Ŷ�� ������ �Ľ��ؼ� Listener�� 
// ��ϵ� ��ü���� �޼����� ������.
//
// ���� �������� �޴� ��Ŷ�� ó���ϰ� �ִ�. Ŭ���̾�Ʈ�� �ڵ�� ���� ����.
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
	 @brief Ŭ���̾�Ʈ���� ��Ŷ�� ������, packet �� Sender�� ���� Id �� �����Ѵ�.
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
				common::format("CTaskLogic:: recv netid: %d, packet id: %d �� �ش��ϴ� NetConnector�� �����ϴ�.\n", 
				packetData.rcvNetId, packetData.packet.GetPacketId()) );
			return RR_CONTINUE;
		}

		UpdateSenderNetId(pCon, type, packetData.packet);

		const ProtocolListenerList &listeners = pCon->GetProtocolListeners();

		// ��� ��Ŷ�� �޾Ƽ� ó���ϴ� �����ʿ��� ��Ŷ�� ������.
		all::Dispatcher allDispatcher;
		allDispatcher.Dispatch(packetData.packet, listeners);
		// 

		const int protocolId = packetData.packet.GetProtocolId();

		// �⺻ �������� ó��
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
				common::format("CTaskLogic %d NetConnector�� protocolId = %d �������� �����ʰ� �����ϴ�.\n", 
				pCon->GetNetId(), protocolId) );
			return RR_CONTINUE;
		}

		IProtocolDispatcher *pDispatcher = CController::Get()->GetDispatcher(protocolId);
		if (!pDispatcher)
		{
			clog::Error( clog::ERROR_PROBLEM,
				common::format("CTaskLogic:: %d �� �ش��ϴ� �������� �����İ� �����ϴ�.\n", 
				protocolId) );
			return RR_CONTINUE;
		}

		if (!pDispatcher->Dispatch(packetData.packet, listeners))
		{
			clog::Error( clog::ERROR_CRITICAL,
				common::format("CTaskLogic netid: %d  packet id: %d NetConnector�� �������� �����ʰ� �����ϴ�.\n", 
				pCon->GetNetId(), packetData.packet.GetPacketId()) );
		}

		return RR_CONTINUE;
	}
}
