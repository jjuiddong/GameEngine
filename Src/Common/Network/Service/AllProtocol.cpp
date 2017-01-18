
#include "stdafx.h"
#include "AllProtocol.h"

using namespace network;
using namespace all;


//------------------------------------------------------------------------
// 모든 패킷을 보낼수있는 함수
//------------------------------------------------------------------------
void all::Protocol::send(netid targetId, const network::SEND_FLAG flag, network::CPacket &packet)
{
	GetNetConnector()->Send(targetId, flag, packet);
}
