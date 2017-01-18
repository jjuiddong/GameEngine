//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\certify_Protocol.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace certify {

using namespace network;
using namespace marshalling;
static const int s2s_Protocol_ID= 900;

class s2s_Protocol : public network::IProtocol
{
public:
	s2s_Protocol() : IProtocol(s2s_Protocol_ID) {}
	void ReqUserLogin(netid targetId, const SEND_FLAG flag, const std::string &id, const std::string &passwd, const std::string &svrType);
	void AckUserLogin(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const certify_key &c_key);
	void ReqUserMoveServer(netid targetId, const SEND_FLAG flag, const std::string &id, const std::string &svrType);
	void AckUserMoveServer(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id, const std::string &svrType);
	void ReqUserLogout(netid targetId, const SEND_FLAG flag, const std::string &id);
	void AckUserLogout(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &id);
};
}
