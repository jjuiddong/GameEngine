
#include "stdafx.h"
#include "MarshallingEtc.h"

using namespace network;
using namespace marshalling;


CPacket& marshalling::operator<<(CPacket& packet, const SHostInfo &rhs)
{
	packet << rhs.ip;
	packet << rhs.portnum;
	return packet;
}


CPacket& marshalling::operator>>(CPacket& packet, SHostInfo &rhs)
{
	packet >> rhs.ip;
	packet >> rhs.portnum;
	return packet;
}
