
#include "stdafx.h"
#include "AllProtocolListener.h"
#include "Network/Controller/Controller.h"

using namespace network;
using namespace all;

//------------------------------------------------------------------------
// ��Ŷ�� �������ݿ� ���� �ش��ϴ� �������� �Լ��� ȣ���Ѵ�.
//------------------------------------------------------------------------
bool all::Dispatcher::Dispatch(CPacket &packet, const ProtocolListenerList &listeners)
{
	CPacket newPacket = packet;
	SEND_LISTENER(ProtocolListener, listeners, recv(packet.GetSenderId(), newPacket) );
	return true;
}
