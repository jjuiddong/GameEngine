#include "basic_ProtocolListener.h"
#include "Network/Controller/Controller.h"

using namespace basic;

static basic::s2c_Dispatcher g_basic_s2c_Dispatcher;

basic::s2c_Dispatcher::s2c_Dispatcher()
	: IProtocolDispatcher(basic::s2c_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool basic::s2c_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 101:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			Error_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, Error(data));
		}
		break;

	case 102:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckLogIn_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.id;
			packet >> data.c_key;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckLogIn(data));
		}
		break;

	case 103:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckLogOut_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.id;
			packet >> data.result;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckLogOut(data));
		}
		break;

	case 104:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckMoveToServer_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.serverName;
			packet >> data.ip;
			packet >> data.port;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckMoveToServer(data));
		}
		break;

	case 105:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckGroupList_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.groups;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckGroupList(data));
		}
		break;

	case 106:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckGroupJoin_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.reqId;
			packet >> data.joinGroupId;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckGroupJoin(data));
		}
		break;

	case 107:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckGroupCreate_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.reqId;
			packet >> data.crGroupId;
			packet >> data.crParentGroupId;
			packet >> data.groupName;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckGroupCreate(data));
		}
		break;

	case 108:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckGroupCreateBlank_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.reqId;
			packet >> data.crGroupId;
			packet >> data.crParentGroupId;
			packet >> data.groupName;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckGroupCreateBlank(data));
		}
		break;

	case 109:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			JoinMember_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.toGroupId;
			packet >> data.fromGroupId;
			packet >> data.userId;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, JoinMember(data));
		}
		break;

	case 110:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<s2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckP2PConnect_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.errorCode;
			packet >> data.state;
			packet >> data.ip;
			packet >> data.port;
			SEND_LISTENER(s2c_ProtocolListener, recvListener, AckP2PConnect(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



static basic::c2s_Dispatcher g_basic_c2s_Dispatcher;

basic::c2s_Dispatcher::c2s_Dispatcher()
	: IProtocolDispatcher(basic::c2s_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool basic::c2s_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 201:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqLogIn_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			packet >> data.passwd;
			packet >> data.c_key;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqLogIn(data));
		}
		break;

	case 202:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqLogOut_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqLogOut(data));
		}
		break;

	case 203:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqMoveToServer_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.serverName;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqMoveToServer(data));
		}
		break;

	case 204:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqGroupList_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.groupid;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqGroupList(data));
		}
		break;

	case 205:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqGroupJoin_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.groupid;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqGroupJoin(data));
		}
		break;

	case 206:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqGroupCreate_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.parentGroupId;
			packet >> data.groupName;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqGroupCreate(data));
		}
		break;

	case 207:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqGroupCreateBlank_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.parentGroupId;
			packet >> data.groupName;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqGroupCreateBlank(data));
		}
		break;

	case 208:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqP2PConnect_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqP2PConnect(data));
		}
		break;

	case 209:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2s_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqP2PConnectTryResult_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.isSuccess;
			SEND_LISTENER(c2s_ProtocolListener, recvListener, ReqP2PConnectTryResult(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



