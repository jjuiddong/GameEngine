#include "farm_ProtocolListener.h"
#include "Network/Controller/Controller.h"

using namespace farm;

static farm::s2c_Dispatcher g_farm_s2c_Dispatcher;

farm::s2c_Dispatcher::s2c_Dispatcher()
	: IProtocolDispatcher(farm::s2c_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool farm::s2c_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 2001:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckSubServerLogin_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckSubServerLogin(data));
		}
		break;

	case 2002:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckSendSubServerP2PCLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckSendSubServerP2PCLink(data));
		}
		break;

	case 2003:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckSendSubServerP2PSLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckSendSubServerP2PSLink(data));
		}
		break;

	case 2004:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckSendSubServerInputLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckSendSubServerInputLink(data));
		}
		break;

	case 2005:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckSendSubServerOutputLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckSendSubServerOutputLink(data));
		}
		break;

	case 2006:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckServerInfoList_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.clientSvrType;
			packet >> data.serverSvrType;
			packet >> data.v;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckServerInfoList(data));
		}
		break;

	case 2007:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckToBindOuterPort_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.bindSubServerSvrType;
			packet >> data.port;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckToBindOuterPort(data));
		}
		break;

	case 2008:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckToBindInnerPort_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.bindSubServerSvrType;
			packet >> data.port;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckToBindInnerPort(data));
		}
		break;

	case 2009:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckSubServerBindComplete_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.bindSubServerSvrType;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckSubServerBindComplete(data));
		}
		break;

	case 2010:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckSubClientConnectComplete_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.bindSubServerSvrType;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckSubClientConnectComplete(data));
		}
		break;

	case 2011:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			BindSubServer_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.bindSubSvrType;
			packet >> data.ip;
			packet >> data.port;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, BindSubServer(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



static farm::c2s_Dispatcher g_farm_c2s_Dispatcher;

farm::c2s_Dispatcher::c2s_Dispatcher()
	: IProtocolDispatcher(farm::c2s_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool farm::c2s_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 2101:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqSubServerLogin_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.svrType;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqSubServerLogin(data));
		}
		break;

	case 2102:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			SendSubServerP2PCLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.v;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, SendSubServerP2PCLink(data));
		}
		break;

	case 2103:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			SendSubServerP2PSLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.v;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, SendSubServerP2PSLink(data));
		}
		break;

	case 2104:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			SendSubServerInputLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.v;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, SendSubServerInputLink(data));
		}
		break;

	case 2105:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			SendSubServerOutputLink_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.v;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, SendSubServerOutputLink(data));
		}
		break;

	case 2106:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqServerInfoList_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.clientSvrType;
			packet >> data.serverSvrType;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqServerInfoList(data));
		}
		break;

	case 2107:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqToBindOuterPort_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.bindSubServerSvrType;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqToBindOuterPort(data));
		}
		break;

	case 2108:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqToBindInnerPort_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.bindSubServerSvrType;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqToBindInnerPort(data));
		}
		break;

	case 2109:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqSubServerBindComplete_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.bindSubServerSvrType;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqSubServerBindComplete(data));
		}
		break;

	case 2110:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqSubClientConnectComplete_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.bindSubServerSvrType;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqSubClientConnectComplete(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



