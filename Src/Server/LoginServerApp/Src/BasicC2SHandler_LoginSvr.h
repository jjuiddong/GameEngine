/**
Name:   BasicC2SProtocolHandler_LoginSvr.h
Author:  jjuiddong
Date:    4/22/2013

	Login server 용 basic protocol 처리 클래스
*/
#pragma once

#include "Network/ProtocolHandler/BasicC2SHandler.h"

#include "NetProtocol/Src/server_network_Protocol.h"
#include "NetProtocol/src/certify_Protocol.h"
#include "NetProtocol/src/certify_ProtocolListener.h"


class CBasicC2SHandler_LoginSvr : public network::CBasicC2SHandler
														, public certify::s2s_ProtocolListener
{
public:
	CBasicC2SHandler_LoginSvr(network::multinetwork::CMultiPlug &certifySvr, network::CServerBasic &svr);
	virtual ~CBasicC2SHandler_LoginSvr();

protected:
	// Network Protocol Handler
	// certify server
	virtual bool AckUserLogin(certify::AckUserLogin_Packet &packet) override;
	virtual bool AckUserLogout(certify::AckUserLogout_Packet &packet) override;
	virtual bool AckUserMoveServer(certify::AckUserMoveServer_Packet &packet) override;

	// client
	virtual bool ReqLogIn(basic::ReqLogIn_Packet &packet) override;
	virtual bool ReqLogOut(basic::ReqLogOut_Packet &packet) override;
	virtual bool ReqMoveToServer(basic::ReqMoveToServer_Packet &packet) override;

private:
	certify::s2s_Protocol	m_CertifyProtocol;
	basic::s2c_Protocol		m_BasicProtocol;
};
