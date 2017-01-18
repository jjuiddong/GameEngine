#include "login_Protocol.h"
using namespace login;

//------------------------------------------------------------------------
// Protocol: AckLobbyIn
//------------------------------------------------------------------------
void login::s2c_Protocol::AckLobbyIn(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 601 );
	packet << errorCode;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



//------------------------------------------------------------------------
// Protocol: ReqLobbyIn
//------------------------------------------------------------------------
void login::c2s_Protocol::ReqLobbyIn(netid targetId, const SEND_FLAG flag)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 701 );
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



