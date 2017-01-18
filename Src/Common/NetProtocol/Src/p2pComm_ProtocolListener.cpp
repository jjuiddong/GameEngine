#include "p2pComm_ProtocolListener.h"
#include "Network/Controller/Controller.h"

using namespace p2pComm;

static p2pComm::c2c_Dispatcher g_p2pComm_c2c_Dispatcher;

p2pComm::c2c_Dispatcher::c2c_Dispatcher()
	: IProtocolDispatcher(p2pComm::c2c_Dispatcher_ID)
{
	CController::Get()->AddDispatcher(this);
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 리스너의 함수를 호출한다.
//------------------------------------------------------------------------
bool p2pComm::c2c_Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 1001:
		{
			ProtocolListenerList recvListener;
			if (!ListenerMatching<c2c_ProtocolListener>(listeners, recvListener))
				return false;

			SetCurrentDispatchPacket( &packet );

			SendData_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_LISTENER(c2c_ProtocolListener, recvListener, SendData(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



