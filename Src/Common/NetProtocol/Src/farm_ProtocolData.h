//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\farm_ProtocolData.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace farm {

using namespace network;
using namespace marshalling;


	struct AckSubServerLogin_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
	};

	struct AckSendSubServerP2PCLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
	};

	struct AckSendSubServerP2PSLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
	};

	struct AckSendSubServerInputLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
	};

	struct AckSendSubServerOutputLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
	};

	struct AckServerInfoList_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string clientSvrType;
		std::string serverSvrType;
		std::vector<SHostInfo> v;
	};

	struct AckToBindOuterPort_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string bindSubServerSvrType;
		int port;
	};

	struct AckToBindInnerPort_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string bindSubServerSvrType;
		int port;
	};

	struct AckSubServerBindComplete_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string bindSubServerSvrType;
	};

	struct AckSubClientConnectComplete_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		error::ERROR_CODE errorCode;
		std::string bindSubServerSvrType;
	};

	struct BindSubServer_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string bindSubSvrType;
		std::string ip;
		int port;
	};





	struct ReqSubServerLogin_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string svrType;
	};

	struct SendSubServerP2PCLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::vector<std::string> v;
	};

	struct SendSubServerP2PSLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::vector<std::string> v;
	};

	struct SendSubServerInputLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::vector<std::string> v;
	};

	struct SendSubServerOutputLink_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::vector<std::string> v;
	};

	struct ReqServerInfoList_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string clientSvrType;
		std::string serverSvrType;
	};

	struct ReqToBindOuterPort_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string bindSubServerSvrType;
	};

	struct ReqToBindInnerPort_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string bindSubServerSvrType;
	};

	struct ReqSubServerBindComplete_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string bindSubServerSvrType;
	};

	struct ReqSubClientConnectComplete_Packet {
		IProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string bindSubServerSvrType;
	};



}
