//------------------------------------------------------------------------
// Name:    LogInListener.h
// Author:  jjuiddong
// Date:    12/29/2012
// 
// �α��� ��Ŷ�� �޾Ƽ� ó���ϴ� �κ�
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
