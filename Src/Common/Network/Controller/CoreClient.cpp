
#include "stdafx.h"
#include "CoreClient.h"
#include "../Service/AllProtocolListener.h"
#include "Controller.h"
#include "../ProtocolHandler/BasicProtocolDispatcher.h"
#include <boost/bind.hpp>


using namespace network;

CCoreClient::CCoreClient(PROCESS_TYPE procType) :
	CPlug(procType)
,	m_ServerNetId(common::GenerateId())
{
	SetIp( "127.0.0.1" );
	SetPort( 2333 );
}

CCoreClient::~CCoreClient() 
{
	Clear();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CCoreClient::Stop()
{
	CController::Get()->StopCoreClient(this);
	return true;
}


//------------------------------------------------------------------------
// 매 프레임마다 호출되어야 하는 함수다.
// 패킷이 서버로 부터 왔는지 검사한다.
//------------------------------------------------------------------------
bool CCoreClient::Proc()
{
	if (!IsConnect())
		return false;

	const timeval t = {0, 0}; // 0 millisecond
	fd_set readSockets;
	readSockets.fd_count = 1;
	readSockets.fd_array[ 0] = GetSocket();
	const int ret = select( readSockets.fd_count, &readSockets, NULL, NULL, &t);
	if (ret != 0 && ret != SOCKET_ERROR)
	{
		char buf[ CPacket::MAX_PACKETSIZE];
		const int result = recv( readSockets.fd_array[ 0], buf, sizeof(buf), 0);
		if (result == SOCKET_ERROR || result == 0) // 받은 패킷사이즈가 0이면 서버와 끊겼다는 의미다.
		{
			CPacketQueue::Get()->PushPacket( 
				CPacketQueue::SPacketData(GetNetId(), 
					DisconnectPacket(GetNetId(), CController::Get()->GetUniqueValue()) ));
		}
		else
		{
			CPacketQueue::Get()->PushPacket( 
				CPacketQueue::SPacketData(GetNetId(), CPacket(SERVER_NETID,buf)) );
		}
	}

	/// Dispatch Packet
	DispatchPacket();

	return true;
}


/**
 @brief Packet 전송
 */
void	CCoreClient::DispatchPacket()
{
	CPacketQueue::SPacketData packetData;
	if (!CPacketQueue::Get()->PopPacket(GetNetId(), packetData))
		return;

	const ProtocolListenerList &listeners = GetProtocolListeners();

	// 모든 패킷을 받아서 처리하는 리스너에게 패킷을 보낸다.
	all::Dispatcher allDispatcher;
	allDispatcher.Dispatch(packetData.packet, listeners);
	// 

	const int protocolId = packetData.packet.GetProtocolId();

	// 기본 프로토콜 처리
	if (protocolId == 0)
	{
		basic_protocol::ClientDispatcher dispatcher;
		dispatcher.Dispatch( packetData.packet, this );
		return;
	}

	if (listeners.empty())
	{
		clog::Error( clog::ERROR_CRITICAL, " CClientCore::DispatchPacket() 프로토콜 리스너가 없습니다. netid: %d\n", GetNetId());
		return;
	}

	IProtocolDispatcher *pDispatcher = CController::Get()->GetDispatcher(protocolId);
	if (!pDispatcher)
	{
		clog::Error( clog::ERROR_WARNING, 
			common::format(" CClientCore::DispatchPacket() %d 에 해당하는 프로토콜 디스패쳐가 없습니다.\n", 
			protocolId) );
	}
	else
	{
		if (!pDispatcher->Dispatch(packetData.packet, listeners))
		{
			clog::Error( clog::ERROR_CRITICAL,
				common::format("CCoreClient %d NetConnector의 프로토콜 리스너가 없습니다.\n", GetNetId()) );
		}
	}

}


//------------------------------------------------------------------------
// 서버와 접속이 끊어질때 호출된다.
//------------------------------------------------------------------------
void CCoreClient::Disconnect()
{
	Close();
	CController::Get()->RemoveCoreClient(this);
	OnDisconnect();
}


/**
@brief  close socket
*/
void CCoreClient::Close()
{
	SetState(SESSIONSTATE_DISCONNECT);
	ClearConnection();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CCoreClient::Clear()
{
	SetState(SESSIONSTATE_DISCONNECT);
	ClearConnection();
}


/**
 @brief Send
 */
bool	CCoreClient::Send(netid netId, const SEND_FLAG flag, CPacket &packet)
{
	if (!IsConnect())
		return false;
	if (netId == SERVER_NETID || (m_ServerNetId == netId))
	{
		if (DISPLAY_PACKET_LOG)
		{
			packet.SetSenderId(GetNetId());
			protocols::DisplayPacket("Send =", packet);
		}

		// send(연결된 소켓, 보낼 버퍼, 버퍼의 길이, 상태값)
		const int result = send(GetSocket(), packet.GetData(), CPacket::MAX_PACKETSIZE, 0);
		if (result == INVALID_SOCKET)
		{
			Disconnect();
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}


//------------------------------------------------------------------------
// 연결된 모든 클라이언트들에게 메세지를 보낸다.
//------------------------------------------------------------------------
bool CCoreClient::SendAll(CPacket &packet)
{
	if (!IsConnect())
		return false;

	const int result = send(GetSocket(), packet.GetData(), CPacket::MAX_PACKETSIZE, 0);
	if (result == INVALID_SOCKET)
	{
		Disconnect();
		return false;
	}
	return true;
}


/**
 @brief Event Connect, call from launcer
 */
void	CCoreClient::OnConnect()
{
	SearchEventTable( CNetEvent(EVT_CONNECT, this) );
}


/**
 @brief Disconnect
 */
void	CCoreClient::OnDisconnect()
{
	SearchEventTable( CNetEvent(EVT_DISCONNECT, this) );
}


/**
 @brief P2P member join
 */
void	CCoreClient::OnMemberJoin(netid netId)
{
	SearchEventTable( CNetEvent(EVT_MEMBER_JOIN, this, netId) );
}


/**
 @brief P2P member leave
 */
void	CCoreClient::OnMemberLeave(netid netId)
{
	SearchEventTable( CNetEvent(EVT_MEMBER_LEAVE, this, netId) );
}
