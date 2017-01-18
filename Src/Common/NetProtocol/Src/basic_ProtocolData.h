//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\basic_ProtocolData.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace basic {

using namespace network;
using namespace marshalling;


	struct Error_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
	};

	struct AckLogIn_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string id;
		certify_key c_key;
	};

	struct AckLogOut_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string id;
		int result;
	};

	struct AckMoveToServer_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string serverName;
		std::string ip;
		int port;
	};

	struct AckGroupList_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		GroupVector groups;
	};

	struct AckGroupJoin_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		netid reqId;
		netid joinGroupId;
	};

	struct AckGroupCreate_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		netid reqId;
		netid crGroupId;
		netid crParentGroupId;
		std::string groupName;
	};

	struct AckGroupCreateBlank_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		netid reqId;
		netid crGroupId;
		netid crParentGroupId;
		std::string groupName;
	};

	struct JoinMember_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		netid toGroupId;
		netid fromGroupId;
		netid userId;
	};

	struct AckP2PConnect_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		P2P_STATE state;
		std::string ip;
		int port;
	};





	struct ReqLogIn_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string id;
		std::string passwd;
		certify_key c_key;
	};

	struct ReqLogOut_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string id;
	};

	struct ReqMoveToServer_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string serverName;
	};

	struct ReqGroupList_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		netid groupid;
	};

	struct ReqGroupJoin_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		netid groupid;
	};

	struct ReqGroupCreate_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		netid parentGroupId;
		std::string groupName;
	};

	struct ReqGroupCreateBlank_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		netid parentGroupId;
		std::string groupName;
	};

	struct ReqP2PConnect_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqP2PConnectTryResult_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		bool isSuccess;
	};



}
