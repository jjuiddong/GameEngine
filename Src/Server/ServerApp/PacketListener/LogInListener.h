//------------------------------------------------------------------------
// Name:    LogInListener.h
// Author:  jjuiddong
// Date:    12/29/2012
// 
// 로그인 패킷을 받아서 처리하는 부분
//------------------------------------------------------------------------
#pragma once

#include "NetCommon/login_ProtocolListener.h"

class CLobbyServer;
class CLogInListener : public login::c2s_ProtocolListener
{
public:
	CLogInListener(CLobbyServer *psvr) : m_pLobbySvr(psvr) {}
protected:
	CLobbyServer	*m_pLobbySvr;
	virtual void	ReqLogin(netid senderId, const std::string &id, const std::string &password) override;

};
