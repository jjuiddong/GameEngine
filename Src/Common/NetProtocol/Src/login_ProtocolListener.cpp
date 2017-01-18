#include "login_ProtocolListener.h"
#include "Network/Controller/Controller.h"

using namespace login;

static login::s2c_Dispatcher g_login_s2c_Dispatcher;

login::s2c_Dispatcher::s2c_Dispatcher()
	: IProtocolDispatcher(login::s2c_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool login::s2c_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 601:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckLobbyIn_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckLobbyIn(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



static login::c2s_Dispatcher g_login_c2s_Dispatcher;

login::c2s_Dispatcher::c2s_Dispatcher()
	: IProtocolDispatcher(login::c2s_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool login::c2s_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 701:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqLobbyIn_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqLobbyIn(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



