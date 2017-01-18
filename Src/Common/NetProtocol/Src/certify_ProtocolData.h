//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\certify_ProtocolData.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace certify {

using namespace network;
using namespace marshalling;


	struct ReqUserLogin_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string id;
		std::string passwd;
		std::string svrType;
	};

	struct AckUserLogin_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string id;
		certify_key c_key;
	};

	struct ReqUserMoveServer_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string id;
		std::string svrType;
	};

	struct AckUserMoveServer_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string id;
		std::string svrType;
	};

	struct ReqUserLogout_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string id;
	};

	struct AckUserLogout_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string id;
	};



}
