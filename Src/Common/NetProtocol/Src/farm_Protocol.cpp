#include "farm_Protocol.h"
using namespace farm;

//------------------------------------------------------------------------
// Protocol: AckSubServerLogin
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckSubServerLogin(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2001 );
	packet << errorCode;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckSendSubServerP2PCLink
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckSendSubServerP2PCLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2002 );
	packet << errorCode;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckSendSubServerP2PSLink
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckSendSubServerP2PSLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2003 );
	packet << errorCode;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckSendSubServerInputLink
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckSendSubServerInputLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2004 );
	packet << errorCode;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckSendSubServerOutputLink
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckSendSubServerOutputLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2005 );
	packet << errorCode;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckServerInfoList
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckServerInfoList(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &clientSvrType, const std::string &serverSvrType, const std::vector<SHostInfo> &v)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2006 );
	packet << errorCode;
	packet << clientSvrType;
	packet << serverSvrType;
	packet << v;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckToBindOuterPort
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckToBindOuterPort(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType, const int &port)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2007 );
	packet << errorCode;
	packet << bindSubServerSvrType;
	packet << port;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckToBindInnerPort
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckToBindInnerPort(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType, const int &port)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2008 );
	packet << errorCode;
	packet << bindSubServerSvrType;
	packet << port;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckSubServerBindComplete
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckSubServerBindComplete(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2009 );
	packet << errorCode;
	packet << bindSubServerSvrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckSubClientConnectComplete
//------------------------------------------------------------------------
void farm::s2c_Protocol::AckSubClientConnectComplete(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2010 );
	packet << errorCode;
	packet << bindSubServerSvrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: BindSubServer
//------------------------------------------------------------------------
void farm::s2c_Protocol::BindSubServer(netid targetId, const SEND_FLAG flag, const std::string &bindSubSvrType, const std::string &ip, const int &port)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2011 );
	packet << bindSubSvrType;
	packet << ip;
	packet << port;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



//------------------------------------------------------------------------
// Protocol: ReqSubServerLogin
//------------------------------------------------------------------------
void farm::c2s_Protocol::ReqSubServerLogin(netid targetId, const SEND_FLAG flag, const std::string &svrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2101 );
	packet << svrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: SendSubServerP2PCLink
//------------------------------------------------------------------------
void farm::c2s_Protocol::SendSubServerP2PCLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2102 );
	packet << v;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: SendSubServerP2PSLink
//------------------------------------------------------------------------
void farm::c2s_Protocol::SendSubServerP2PSLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2103 );
	packet << v;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: SendSubServerInputLink
//------------------------------------------------------------------------
void farm::c2s_Protocol::SendSubServerInputLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2104 );
	packet << v;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: SendSubServerOutputLink
//------------------------------------------------------------------------
void farm::c2s_Protocol::SendSubServerOutputLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2105 );
	packet << v;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqServerInfoList
//------------------------------------------------------------------------
void farm::c2s_Protocol::ReqServerInfoList(netid targetId, const SEND_FLAG flag, const std::string &clientSvrType, const std::string &serverSvrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2106 );
	packet << clientSvrType;
	packet << serverSvrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqToBindOuterPort
//------------------------------------------------------------------------
void farm::c2s_Protocol::ReqToBindOuterPort(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2107 );
	packet << bindSubServerSvrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqToBindInnerPort
//------------------------------------------------------------------------
void farm::c2s_Protocol::ReqToBindInnerPort(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2108 );
	packet << bindSubServerSvrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqSubServerBindComplete
//------------------------------------------------------------------------
void farm::c2s_Protocol::ReqSubServerBindComplete(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2109 );
	packet << bindSubServerSvrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqSubClientConnectComplete
//------------------------------------------------------------------------
void farm::c2s_Protocol::ReqSubClientConnectComplete(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2110 );
	packet << bindSubServerSvrType;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



