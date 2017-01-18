#include "stdafx.h"
#include "ClientBasic.h"
#include "../Controller/P2PClient.h"
#include "../Controller/Controller.h"
#include "../Service/AllProtocolListener.h"
#include "../Controller/CoreClient.h"

using namespace network;


CClientBasic::CClientBasic(PROCESS_TYPE procType) :
	CPlug(procType)
,	m_pP2p(NULL)
,	m_pConnectSvr(NULL)
{
	m_pConnectSvr = new CCoreClient(SERVICE_CHILD_THREAD);
	m_pConnectSvr->EventConnect( this, EVT_CONNECT, NetEventHandler(CClientBasic::OnConnect) );
	m_pConnectSvr->EventConnect( this, EVT_DISCONNECT, NetEventHandler(CClientBasic::OnDisconnect) );
	AddChild(m_pConnectSvr);

	m_pP2p = new CP2PClient(SERVICE_CHILD_THREAD);
	m_pP2p->EventConnect( this, EVT_CONNECT, NetEventHandler(CClientBasic::OnConnect) );
	m_pP2p->EventConnect( this, EVT_DISCONNECT, NetEventHandler(CClientBasic::OnDisconnect) );
	m_pP2p->EventConnect( this, EVT_MEMBER_JOIN, NetEventHandler(CClientBasic::OnMemberJoin) );
	m_pP2p->EventConnect( this, EVT_MEMBER_LEAVE, NetEventHandler(CClientBasic::OnMemberLeave) );
	AddChild(m_pP2p);

}

CClientBasic::~CClientBasic() 
{
	Clear();
}


//------------------------------------------------------------------------
// Client 기능 멈춤
// 모든 소켓이 초기화 된다.
//------------------------------------------------------------------------
bool CClientBasic::Stop()
{
	CController::Get()->StopClient(this);
	if (m_pConnectSvr)	
		m_pConnectSvr->Stop();
	if (m_pP2p)
		m_pP2p->Stop();
	return true;
}


//------------------------------------------------------------------------
// PROCESS_TYPE이 USER_LOOP일 때, 매 프레임마다 호출되어야 하는 함수다.
// 패킷이 서버로 부터 왔는지 검사한다.
//------------------------------------------------------------------------
bool CClientBasic::Proc()
{
	if (m_pConnectSvr)	
		m_pConnectSvr->Proc();
	if (m_pP2p)
		m_pP2p->Proc();
	return true;
}


//------------------------------------------------------------------------
// 서버와 접속이 끊어질때 호출된다.
//------------------------------------------------------------------------
void CClientBasic::Disconnect()
{
	CController::Get()->RemoveClient(this);
	if (m_pConnectSvr)
		m_pConnectSvr->Disconnect();
	if (m_pP2p)
		m_pP2p->Disconnect();
}


/**
@brief  close socket
*/
void	CClientBasic::Close()
{
	if (m_pConnectSvr)
		m_pConnectSvr->Close();
	if (m_pP2p)
		m_pP2p->Close();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CClientBasic::Clear()
{
	Stop();
	SAFE_DELETE(m_pConnectSvr);
	SAFE_DELETE(m_pP2p);
}


//------------------------------------------------------------------------
// P2p와 Server에 접속하는 coreClient에게도 Protocol을 설정한다.
//------------------------------------------------------------------------
//bool	CClientBasic::AddProtocolListener(ProtocolListenerPtr pListener)
//{
//	if (!CPlug::AddProtocolListener(pListener))
//		return false;
//	if (m_pConnectSvr)
//		m_pConnectSvr->AddProtocolListener(pListener);
//	if (m_pP2p)
//		m_pP2p->AddProtocolListener(pListener);
//	return true;
//}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
//bool	CClientBasic::RemoveProtocolListener(ProtocolListenerPtr pListener)
//{
//	if (!CPlug::RemoveProtocolListener(pListener))
//		return false;
//	if (m_pConnectSvr)
//		m_pConnectSvr->RemoveProtocolListener(pListener);
//	if (m_pP2p)
//		m_pP2p->RemoveProtocolListener(pListener);
//	return true;
//}


/**
 @brief Send Packet
 */
bool	CClientBasic::Send(netid netId, const SEND_FLAG flag, CPacket &packet)
{
	if (P2P_NETID == netId)
	{
		return SendP2P(packet);
	}
	else
	{
		if (!m_pConnectSvr)
			return false;
		return m_pConnectSvr->Send(netId, flag, packet);
	}
}


/**
 @brief Send P2P Packet
 */
bool	CClientBasic::SendP2P(CPacket &packet)
{
	if (!m_pP2p)
		return false;
	return m_pP2p->Send(P2P_NETID, SEND_TARGET, packet);
}


//------------------------------------------------------------------------
// 연결된 모든 클라이언트들에게 메세지를 보낸다.
//------------------------------------------------------------------------
bool CClientBasic::SendAll(CPacket &packet)
{
	// 아직 아무것도 없음
	return true;
}


//------------------------------------------------------------------------
// 서버와 연결되어 있다면 true를 리턴한다.
//------------------------------------------------------------------------
bool	CClientBasic::IsConnect() const 
{
	return m_pConnectSvr && m_pConnectSvr->IsConnect();
}


/**
 @brief if P2P Host Client is return true, or false
 */
bool	CClientBasic::IsP2PHostClient() const
{
	RETV(!m_pP2p, false);
	return m_pP2p->IsHostClient();
}


//------------------------------------------------------------------------
// Connect Event Handler
//------------------------------------------------------------------------
void	CClientBasic::OnConnect(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	if (m_pConnectSvr == event.GetEventObject())
	{
		event.Skip();
		SearchEventTable( CNetEvent(EVT_CONNECT, this) );		
	}
	else if (m_pP2p ==  event.GetEventObject())
	{
		event.Skip();
		SearchEventTable( CNetEvent(EVT_P2P_CONNECT, this) );
	}
}


/**
 @brief 
 */
void	CClientBasic::OnDisconnect(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	if (m_pConnectSvr == event.GetEventObject())
	{
		event.Skip();
		SearchEventTable( CNetEvent(EVT_DISCONNECT, this) );
	}
	else if (m_pP2p == event.GetEventObject())
	{
		event.Skip();
		SearchEventTable( CNetEvent(EVT_P2P_DISCONNECT, this) );
	}
}


/**
 @brief P2P join client
 */
void	CClientBasic::OnMemberJoin(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	event.Skip();
	SearchEventTable( CNetEvent(EVT_MEMBER_JOIN, this, event.GetNetId()) );
}


/**
 @brief P2P leave client
 */
void	CClientBasic::OnMemberLeave(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	event.Skip();
	SearchEventTable( CNetEvent(EVT_MEMBER_LEAVE, this, event.GetNetId()) );
}
