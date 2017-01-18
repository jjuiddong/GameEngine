//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\login_ProtocolData.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace login {

using namespace network;
using namespace marshalling;


	struct AckLobbyIn_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
	};





	struct ReqLobbyIn_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
	};



}
