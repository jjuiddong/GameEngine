//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\p2pComm_Protocol.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace p2pComm {

using namespace network;
using namespace marshalling;
static const int c2c_Protocol_ID= 1000;

class c2c_Protocol : public network::IProtocol
{
public:
	c2c_Protocol() : IProtocol(c2c_Protocol_ID) {}
	void SendData(netid targetId, const SEND_FLAG flag);
};
}
