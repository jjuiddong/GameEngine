//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\server_network_Protocol.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace server_network {

using namespace network;
using namespace marshalling;
static const int s2s_Protocol_ID= 800;

class s2s_Protocol : public network::IProtocol
{
public:
	s2s_Protocol() : IProtocol(s2s_Protocol_ID) {}
	void ReqMovePlayer(netid targetId, const SEND_FLAG flag, const std::string &id, const certify_key &c_key, const netid &groupId, const std::string &ip, const int &port);
	void AckMovePlayer(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const netid &groupId, const std::string &ip, const int &port);
	void ReqMovePlayerCancel(netid targetId, const SEND_FLAG flag, const std::string &id);
	void AckMovePlayerCancel(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id);
	void ReqCreateGroup(netid targetId, const SEND_FLAG flag, const std::string &name, const netid &groupId, const netid &reqPlayerId);
	void AckCreateGroup(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &name, const netid &groupId, const netid &reqPlayerId);
	void SendServerInfo(netid targetId, const SEND_FLAG flag, const std::string &svrType, const std::string &ip, const int &port, const int &userCount);
};
}
