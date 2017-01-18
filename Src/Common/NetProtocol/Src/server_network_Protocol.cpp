#include "server_network_Protocol.h"
using namespace server_network;

//------------------------------------------------------------------------
// Protocol: ReqMovePlayer
//------------------------------------------------------------------------
void server_network::s2s_Protocol::ReqMovePlayer(netid targetId, const SEND_FLAG flag, const std::string &id, const certify_key &c_key, const netid &groupId, const std::string &ip, const int &port)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 801 );
	packet << id;
	packet << c_key;
	packet << groupId;
	packet << ip;
	packet << port;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckMovePlayer
//------------------------------------------------------------------------
void server_network::s2s_Protocol::AckMovePlayer(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const netid &groupId, const std::string &ip, const int &port)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 802 );
	packet << errorCode;
	packet << id;
	packet << groupId;
	packet << ip;
	packet << port;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqMovePlayerCancel
//------------------------------------------------------------------------
void server_network::s2s_Protocol::ReqMovePlayerCancel(netid targetId, const SEND_FLAG flag, const std::string &id)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 803 );
	packet << id;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckMovePlayerCancel
//------------------------------------------------------------------------
void server_network::s2s_Protocol::AckMovePlayerCancel(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 804 );
	packet << errorCode;
	packet << id;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqCreateGroup
//------------------------------------------------------------------------
void server_network::s2s_Protocol::ReqCreateGroup(netid targetId, const SEND_FLAG flag, const std::string &name, const netid &groupId, const netid &reqPlayerId)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 805 );
	packet << name;
	packet << groupId;
	packet << reqPlayerId;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckCreateGroup
//------------------------------------------------------------------------
void server_network::s2s_Protocol::AckCreateGroup(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &name, const netid &groupId, const netid &reqPlayerId)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 806 );
	packet << errorCode;
	packet << name;
	packet << groupId;
	packet << reqPlayerId;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: SendServerInfo
//------------------------------------------------------------------------
void server_network::s2s_Protocol::SendServerInfo(netid targetId, const SEND_FLAG flag, const std::string &svrType, const std::string &ip, const int &port, const int &userCount)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 807 );
	packet << svrType;
	packet << ip;
	packet << port;
	packet << userCount;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



