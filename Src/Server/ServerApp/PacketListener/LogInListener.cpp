
#include "stdafx.h"
#include "LogInListener.h"
//#include "NetCommon/login_ProtocolListener.cpp"
#include "../Lib/LobbyServer.h"


//------------------------------------------------------------------------
// �α��� ��û
//------------------------------------------------------------------------
void CLogInListener::ReqLogin(netid senderId, const std::string &id, const std::string &password)
{
	m_pLobbySvr->GetLogInProtocol().AckLogin(senderId, id, 1 );
}
