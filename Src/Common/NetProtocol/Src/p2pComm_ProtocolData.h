//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\p2pComm_ProtocolData.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace p2pComm {

using namespace network;
using namespace marshalling;


	struct SendData_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
	};



}
