
#include "stdafx.h"
#include "server.h"
#include "NetProtocol/Src/basic_Protocol.h"
#include "../ProtocolHandler/BasicC2SHandler.h"


using namespace network;

CServer::CServer() : 
	multinetwork::CMultiPlugDelegation()
,	m_pBasicProtocol(NULL)
,	m_pBasicPrtHandler(NULL)
{

}

CServer::~CServer()
{
	//RemoveProtocolListener(m_pBasicPrtHandler);
	//SAFE_DELETE(m_pBasicPrtHandler);
	SAFE_DELETE(m_pBasicProtocol);
}


/**
 @brief 
 */
void	CServer::OnConnectMultiPlug()
{
	m_pBasicProtocol = new basic::s2c_Protocol();
	RegisterProtocol(m_pBasicProtocol);

	//m_pBasicPrtHandler = new CBasicC2SProtocolHandler(*GetServer());
	//AddProtocolListener(m_pBasicPrtHandler);

	NETEVENT_CONNECT(EVT_CLIENT_JOIN, CServer, CServer::OnClientJoin);
	NETEVENT_CONNECT(EVT_CLIENT_LEAVE, CServer, CServer::OnClientLeave);

}


/**
 @brief Join Client Event
 */
void CServer::OnClientJoin(CNetEvent &event)
{

}


/**
 @brief Leave Client Event
 */
void CServer::OnClientLeave(CNetEvent &event)
{
	RET(!GetServer());

	GroupPtr pGroup = GetServer()->GetRootGroup().GetChildFromPlayer( event.GetNetId() );
	RET(!pGroup);

	m_pBasicProtocol->JoinMember(pGroup->GetNetId(), SEND_T_V, INVALID_NETID, 
		pGroup->GetNetId(), event.GetNetId() );	
}

