//------------------------------------------------------------------------
// Name:    D:\Project\GitHub\MadSoccer\Src\Common\NetProtocol\Src\farm_Protocol.h
// Author:  ProtocolCompiler (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace farm {

using namespace network;
using namespace marshalling;
static const int s2c_Protocol_ID= 2000;

class s2c_Protocol : public network::IProtocol
{
public:
	s2c_Protocol() : IProtocol(s2c_Protocol_ID) {}
	void AckSubServerLogin(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode);
	void AckSendSubServerP2PCLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode);
	void AckSendSubServerP2PSLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode);
	void AckSendSubServerInputLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode);
	void AckSendSubServerOutputLink(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode);
	void AckServerInfoList(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &clientSvrType, const std::string &serverSvrType, const std::vector<SHostInfo> &v);
	void AckToBindOuterPort(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType, const int &port);
	void AckToBindInnerPort(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType, const int &port);
	void AckSubServerBindComplete(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType);
	void AckSubClientConnectComplete(netid targetId, const SEND_FLAG flag, const error::ERROR_CODE &errorCode, const std::string &bindSubServerSvrType);
	void BindSubServer(netid targetId, const SEND_FLAG flag, const std::string &bindSubSvrType, const std::string &ip, const int &port);
};
static const int c2s_Protocol_ID= 2100;

class c2s_Protocol : public network::IProtocol
{
public:
	c2s_Protocol() : IProtocol(c2s_Protocol_ID) {}
	void ReqSubServerLogin(netid targetId, const SEND_FLAG flag, const std::string &svrType);
	void SendSubServerP2PCLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v);
	void SendSubServerP2PSLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v);
	void SendSubServerInputLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v);
	void SendSubServerOutputLink(netid targetId, const SEND_FLAG flag, const std::vector<std::string> &v);
	void ReqServerInfoList(netid targetId, const SEND_FLAG flag, const std::string &clientSvrType, const std::string &serverSvrType);
	void ReqToBindOuterPort(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType);
	void ReqToBindInnerPort(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType);
	void ReqSubServerBindComplete(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType);
	void ReqSubClientConnectComplete(netid targetId, const SEND_FLAG flag, const std::string &bindSubServerSvrType);
};
}
