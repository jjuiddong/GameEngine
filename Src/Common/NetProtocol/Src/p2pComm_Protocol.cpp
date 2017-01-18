#include "p2pComm_Protocol.h"
using namespace p2pComm;

//------------------------------------------------------------------------
// Protocol: SendData
//------------------------------------------------------------------------
void p2pComm::c2c_Protocol::SendData(netid targetId, const SEND_FLAG flag)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1001 );
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



