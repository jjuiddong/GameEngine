#include "certify_Protocol.h"
using namespace certify;

//------------------------------------------------------------------------
// Protocol: ReqUserLogin
//------------------------------------------------------------------------
void certify::s2s_Protocol::ReqUserLogin(netid targetId, const SEND_FLAG flag, const std::string &id, const std::string &passwd, const std::string &svrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 901 );
	packet << id;
	packet << passwd;
	packet << svrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckUserLogin
//------------------------------------------------------------------------
void certify::s2s_Protocol::AckUserLogin(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const certify_key &c_key)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 902 );
	packet << errorCode;
	packet << id;
	packet << c_key;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqUserMoveServer
//------------------------------------------------------------------------
void certify::s2s_Protocol::ReqUserMoveServer(netid targetId, const SEND_FLAG flag, const std::string &id, const std::string &svrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 903 );
	packet << id;
	packet << svrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckUserMoveServer
//------------------------------------------------------------------------
void certify::s2s_Protocol::AckUserMoveServer(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const std::string &svrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 904 );
	packet << errorCode;
	packet << id;
	packet << svrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqUserLogout
//------------------------------------------------------------------------
void certify::s2s_Protocol::ReqUserLogout(netid targetId, const SEND_FLAG flag, const std::string &id)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 905 );
	packet << id;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckUserLogout
//------------------------------------------------------------------------
void certify::s2s_Protocol::AckUserLogout(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 906 );
	packet << errorCode;
	packet << id;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



