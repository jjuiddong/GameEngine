//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\login_Protocol.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace login {

using namespace network;
using namespace marshalling;
static const int s2c_Protocol_ID= 600;

class s2c_Protocol : public network::IProtocol
{
public:
	s2c_Protocol() : IProtocol(s2c_Protocol_ID) {}
	void AckLobbyIn(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode);
};
static const int c2s_Protocol_ID= 700;

class c2s_Protocol : public network::IProtocol
{
public:
	c2s_Protocol() : IProtocol(c2s_Protocol_ID) {}
	void ReqLobbyIn(netid targetId, const SEND_FLAG flag);
};
}
