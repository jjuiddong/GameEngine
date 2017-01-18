#include "basic_Protocol.h"
using namespace basic;

//------------------------------------------------------------------------
// Protocol: Error
//------------------------------------------------------------------------
void basic::s2c_Protocol::Error(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 101 );
	packet << errorCode;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckLogIn
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckLogIn(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const certify_key &c_key)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 102 );
	packet << errorCode;
	packet << id;
	packet << c_key;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckLogOut
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckLogOut(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const int &result)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 103 );
	packet << errorCode;
	packet << id;
	packet << result;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckMoveToServer
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckMoveToServer(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &serverName, const std::string &ip, const int &port)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 104 );
	packet << errorCode;
	packet << serverName;
	packet << ip;
	packet << port;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckGroupList
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckGroupList(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const GroupVector &groups)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 105 );
	packet << errorCode;
	packet << groups;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckGroupJoin
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckGroupJoin(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const netid &reqId, const netid &joinGroupId)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 106 );
	packet << errorCode;
	packet << reqId;
	packet << joinGroupId;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckGroupCreate
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckGroupCreate(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const netid &reqId, const netid &crGroupId, const netid &crParentGroupId, const std::string &groupName)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 107 );
	packet << errorCode;
	packet << reqId;
	packet << crGroupId;
	packet << crParentGroupId;
	packet << groupName;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckGroupCreateBlank
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckGroupCreateBlank(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const netid &reqId, const netid &crGroupId, const netid &crParentGroupId, const std::string &groupName)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 108 );
	packet << errorCode;
	packet << reqId;
	packet << crGroupId;
	packet << crParentGroupId;
	packet << groupName;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: JoinMember
//------------------------------------------------------------------------
void basic::s2c_Protocol::JoinMember(netid targetId, const SEND_FLAG flag, const netid &toGroupId, const netid &fromGroupId, const netid &userId)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 109 );
	packet << toGroupId;
	packet << fromGroupId;
	packet << userId;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: AckP2PConnect
//------------------------------------------------------------------------
void basic::s2c_Protocol::AckP2PConnect(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const P2P_STATE &state, const std::string &ip, const int &port)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 110 );
	packet << errorCode;
	packet << state;
	packet << ip;
	packet << port;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



//------------------------------------------------------------------------
// Protocol: ReqLogIn
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqLogIn(netid targetId, const SEND_FLAG flag, const std::string &id, const std::string &passwd, const certify_key &c_key)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 201 );
	packet << id;
	packet << passwd;
	packet << c_key;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqLogOut
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqLogOut(netid targetId, const SEND_FLAG flag, const std::string &id)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 202 );
	packet << id;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqMoveToServer
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqMoveToServer(netid targetId, const SEND_FLAG flag, const std::string &serverName)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 203 );
	packet << serverName;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqGroupList
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqGroupList(netid targetId, const SEND_FLAG flag, const netid &groupid)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 204 );
	packet << groupid;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqGroupJoin
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqGroupJoin(netid targetId, const SEND_FLAG flag, const netid &groupid)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 205 );
	packet << groupid;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqGroupCreate
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqGroupCreate(netid targetId, const SEND_FLAG flag, const netid &parentGroupId, const std::string &groupName)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 206 );
	packet << parentGroupId;
	packet << groupName;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqGroupCreateBlank
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqGroupCreateBlank(netid targetId, const SEND_FLAG flag, const netid &parentGroupId, const std::string &groupName)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 207 );
	packet << parentGroupId;
	packet << groupName;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqP2PConnect
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqP2PConnect(netid targetId, const SEND_FLAG flag)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 208 );
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqP2PConnectTryResult
//------------------------------------------------------------------------
void basic::c2s_Protocol::ReqP2PConnectTryResult(netid targetId, const SEND_FLAG flag, const bool &isSuccess)
{
	CPacket packet;
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 209 );
	packet << isSuccess;
	packet.EndPack();
	GetNetConnector()->Send(targetId, flag, packet);
}



