#include "certify_ProtocolListener.h"
#include "Network/Controller/Controller.h"

using namespace certify;

static certify::s2s_Dispatcher g_certify_s2s_Dispatcher;

certify::s2s_Dispatcher::s2s_Dispatcher()
	: IProtocolDispatcher(certify::s2s_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool certify::s2s_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 901:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqUserLogin_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			packet >> data.passwd;
			packet >> data.svrType;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, ReqUserLogin(data));
		}
		break;

	case 902:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckUserLogin_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.id;
			packet >> data.c_key;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, AckUserLogin(data));
		}
		break;

	case 903:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqUserMoveServer_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			packet >> data.svrType;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, ReqUserMoveServer(data));
		}
		break;

	case 904:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckUserMoveServer_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.id;
			packet >> data.svrType;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, AckUserMoveServer(data));
		}
		break;

	case 905:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqUserLogout_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, ReqUserLogout(data));
		}
		break;

	case 906:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckUserLogout_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.id;
			SEND_LISTENER(s2s_ProtocolListener, recvListener, AckUserLogout(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



