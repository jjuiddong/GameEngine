
#include "stdafx.h"
#include "Session.h"

using namespace network;


CSession::CSession() : 
	m_Socket(0)
	,	m_State(SESSIONSTATE_DISCONNECT)
	,	m_Id(common::GenerateId()) 
	,	m_P2PState(P2P_CLIENT)
{

}

CSession::~CSession()  
{ 
	closesocket(m_Socket); 
}


/**
 @brief 
 */
void	CSession::ClearConnection()
{
	closesocket(m_Socket);
	m_Socket = 0;
}


/**
 @brief IsSendable
 */
bool	CSession::IsConnect() const 
{ 
	switch (m_State)
	{
	case SESSIONSTATE_DISCONNECT: return false;

	case SESSIONSTATE_LOGIN_WAIT: // go to next case
	case SESSIONSTATE_LOGIN: // got next case
	case SESSIONSTATE_LOGOUT_WAIT:
		return true;
	}
	return false;
}
