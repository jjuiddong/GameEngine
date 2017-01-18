//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\basic_Protocol.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace basic {

using namespace network;
using namespace marshalling;
static const int s2c_Protocol_ID= 100;

class s2c_Protocol : public network::IProtocol
{
public:
	s2c_Protocol() : IProtocol(s2c_Protocol_ID) {}
	void Error(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode);
	void AckLogIn(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const certify_key &c_key);
	void AckLogOut(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const int &result);
	void AckMoveToServer(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &serverName, const std::string &ip, const int &port);
	void AckGroupList(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const GroupVector &groups);
	void AckGroupJoin(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const netid &reqId, const netid &joinGroupId);
	void AckGroupCreate(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const netid &reqId, const netid &crGroupId, const netid &crParentGroupId, const std::string &groupName);
	void AckGroupCreateBlank(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const netid &reqId, const netid &crGroupId, const netid &crParentGroupId, const std::string &groupName);
	void JoinMember(netid targetId, const SEND_FLAG flag, const netid &toGroupId, const netid &fromGroupId, const netid &userId);
	void AckP2PConnect(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const P2P_STATE &state, const std::string &ip, const int &port);
};
static const int c2s_Protocol_ID= 200;

class c2s_Protocol : public network::IProtocol
{
public:
	c2s_Protocol() : IProtocol(c2s_Protocol_ID) {}
	void ReqLogIn(netid targetId, const SEND_FLAG flag, const std::string &id, const std::string &passwd, const certify_key &c_key);
	void ReqLogOut(netid targetId, const SEND_FLAG flag, const std::string &id);
	void ReqMoveToServer(netid targetId, const SEND_FLAG flag, const std::string &serverName);
	void ReqGroupList(netid targetId, const SEND_FLAG flag, const netid &groupid);
	void ReqGroupJoin(netid targetId, const SEND_FLAG flag, const netid &groupid);
	void ReqGroupCreate(netid targetId, const SEND_FLAG flag, const netid &parentGroupId, const std::string &groupName);
	void ReqGroupCreateBlank(netid targetId, const SEND_FLAG flag, const netid &parentGroupId, const std::string &groupName);
	void ReqP2PConnect(netid targetId, const SEND_FLAG flag);
	void ReqP2PConnectTryResult(netid targetId, const SEND_FLAG flag, const bool &isSuccess);
};
}
