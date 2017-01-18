
#include "stdafx.h"
#include "BasicC2SHandler_LoginSvr.h"
#include "Network/ErrReport/ErrorCheck.h"

using namespace network;

CBasicC2SHandler_LoginSvr::CBasicC2SHandler_LoginSvr(multinetwork::CMultiPlug &certifySvr, 
	CServerBasic &svr) :
	network::CBasicC2SHandler(svr)
{
	svr.RegisterProtocol(&m_BasicProtocol);
	certifySvr.RegisterProtocol( &m_CertifyProtocol );
	certifySvr.AddProtocolListener(this);

}

CBasicC2SHandler_LoginSvr::~CBasicC2SHandler_LoginSvr()
{

}


/**
 @brief ReqLogIn
 */
bool CBasicC2SHandler_LoginSvr::ReqLogIn(basic::ReqLogIn_Packet &packet)
{
	if (!CBasicC2SHandler::ReqLogIn(packet))
		return false;

	m_CertifyProtocol.ReqUserLogin(SERVER_NETID, SEND_T, packet.id, packet.passwd, "loginsvr");
	return true;
}


/**
 @brief ReqLogOut
 */
bool CBasicC2SHandler_LoginSvr::ReqLogOut(basic::ReqLogOut_Packet &packet)
{

	return true;
}


/**
 @brief ReqMoveToServer
 */
bool CBasicC2SHandler_LoginSvr::ReqMoveToServer(basic::ReqMoveToServer_Packet &packet)
{
	CSession *pClient = network::CheckClientNetId( &GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher );
	RETV(!pClient, false);

	// 유저가 서버 이동이 가능한지 판단~

	// 이동할 서버 검색, 로그인 서버는 lobby Server 로만 이동할 수 있다.
	if (packet.serverName != "lobbysvr" )
	{
		clog::Error( clog::ERROR_PROBLEM, "ReqMoveToServer Error!! invalid servername name=%s", 
			packet.serverName.c_str());
		m_BasicProtocol.AckMoveToServer(packet.senderId, SEND_T, error::ERR_MOVETOSERVER_INVALID_SERVERNAME, packet.serverName, "", 0);
		return false;
	}
	
	MultiPlugPtr pNetGroupCtrl = multinetwork::CMultiNetwork::Get()->GetMultiPlug("lobbysvr");
	if (!pNetGroupCtrl)
	{
		clog::Error( clog::ERROR_PROBLEM, "ReqMoveToServer Error!! not found server group " );
		m_BasicProtocol.AckMoveToServer(packet.senderId, SEND_T, error::ERR_MOVETOSERVER_NOT_FOUND_SERVER, packet.serverName, "", 0);
		return false;
	}
	
	return true;
}


/**
 @brief From CertifyServer 
		인증서버로 부터 받는다.
		ReqLogin -> ReqUserLogin -> AckUserLogin
 */
bool CBasicC2SHandler_LoginSvr::AckUserLogin(certify::AckUserLogin_Packet &packet)
{
	CSession *pClient = network::CheckClientId( &GetServer(), packet.id, 0, &m_BasicProtocol, packet.pdispatcher );
	RETV(!pClient, false);

	if (packet.errorCode != error::ERR_SUCCESS)
	{
		clog::Error( clog::ERROR_PROBLEM, "AckUserId Error!! client generate user id Error id=%s", packet.id.c_str());
		m_BasicProtocol.AckLogIn(pClient->GetNetId(), SEND_T, packet.errorCode, packet.id, 0);
		return false;
	}

	pClient->SetState(SESSIONSTATE_LOGIN); // login state
	pClient->SetCertifyKey( packet.c_key );
	m_BasicProtocol.AckLogIn(pClient->GetNetId(), SEND_T, packet.errorCode, packet.id, packet.c_key );
	return true;
}


/**
 @brief AckUserMoveServer
 */
bool CBasicC2SHandler_LoginSvr::AckUserMoveServer(certify::AckUserMoveServer_Packet &packet)
{
	
	return true;
}


/**
 @brief AckUserLogout
 */
bool CBasicC2SHandler_LoginSvr::AckUserLogout(certify::AckUserLogout_Packet &packet)
{

	return true;
}
