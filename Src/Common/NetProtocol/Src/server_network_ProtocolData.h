//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\server_network_ProtocolData.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace server_network {

using namespace network;
using namespace marshalling;


	struct ReqMovePlayer_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string id;
		certify_key c_key;
		netid groupId;
		std::string ip;
		int port;
	};

	struct AckMovePlayer_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string id;
		netid groupId;
		std::string ip;
		int port;
	};

	struct ReqMovePlayerCancel_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string id;
	};

	struct AckMovePlayerCancel_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string id;
	};

	struct ReqCreateGroup_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string name;
		netid groupId;
		netid reqPlayerId;
	};

	struct AckCreateGroup_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string name;
		netid groupId;
		netid reqPlayerId;
	};

	struct SendServerInfo_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string svrType;
		std::string ip;
		int port;
		int userCount;
	};



}
