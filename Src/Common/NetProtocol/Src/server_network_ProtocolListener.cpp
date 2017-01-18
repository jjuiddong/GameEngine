#include "server_network_ProtocolListener.h"
#include "Network/Controller/Controller.h"

using namespace server_network;

static server_network::s2s_Dispatcher g_server_network_s2s_Dispatcher;

server_network::s2s_Dispatcher::s2s_Dispatcher()
	: IProtocolDispatcher(server_network::s2s_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool server_network::s2s_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 801:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqMovePlayer_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			packet >> data.c_key;
			packet >> data.groupId;
			packet >> data.ip;
			packet >> data.port;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, ReqMovePlayer(data));
		}
		break;

	case 802:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckMovePlayer_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.id;
			packet >> data.groupId;
			packet >> data.ip;
			packet >> data.port;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, AckMovePlayer(data));
		}
		break;

	case 803:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqMovePlayerCancel_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, ReqMovePlayerCancel(data));
		}
		break;

	case 804:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckMovePlayerCancel_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.id;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, AckMovePlayerCancel(data));
		}
		break;

	case 805:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqCreateGroup_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.name;
			packet >> data.groupId;
			packet >> data.reqPlayerId;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, ReqCreateGroup(data));
		}
		break;

	case 806:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckCreateGroup_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.name;
			packet >> data.groupId;
			packet >> data.reqPlayerId;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, AckCreateGroup(data));
		}
		break;

	case 807:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			SendServerInfo_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.svrType;
			packet >> data.ip;
			packet >> data.port;
			packet >> data.userCount;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, SendServerInfo(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



