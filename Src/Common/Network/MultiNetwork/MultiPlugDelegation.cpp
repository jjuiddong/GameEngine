
#include "stdafx.h"
#include "MultiPlugDelegation.h"
#include "MultiPlug.h"

using namespace network;
using namespace network::multinetwork;


CMultiPlugDelegation::CMultiPlugDelegation() :
	CPlug(SERVICE_SEPERATE_THREAD)
	//m_SvrType(linkSvrType)
,	m_pMultiPlug(NULL)	
{
}

CMultiPlugDelegation::~CMultiPlugDelegation()
{
}

void	 CMultiPlugDelegation::SetMultiPlug(MultiPlugPtr ptr) 
{ 
	m_pMultiPlug = ptr;
	//CPlug *p = dynamic_cast<CPlug*>(ptr.Get());
	//CPlugLinker::SetPlug( p); 
	//if (p)
	if (ptr)
		OnConnectMultiPlug(); /// Call Event Handler
}

MultiPlugPtr CMultiPlugDelegation::GetMultiPlug() const 
{ 
	//RETV(!GetPlug(), NULL);
	//return dynamic_cast<CMultiPlug*>(GetPlug().Get()); 
	return m_pMultiPlug;
}


/**
 @brief 
 */
CServerBasic* CMultiPlugDelegation::GetServer()
{
	RETV(!GetMultiPlug(), NULL);
	return GetMultiPlug()->GetServer();
}


/**
 @brief 
 */
const CoreClients_V& CMultiPlugDelegation::GetClients()
{
	static CoreClients_V v;
	RETV(!GetMultiPlug(), v);
	return GetMultiPlug()->GetClients();
}


/**
 @brief 
 */
CoreClientPtr CMultiPlugDelegation::GetPlayer(netid netId)
{
	RETV(!GetMultiPlug(), NULL);	
	return GetMultiPlug()->GetClient(netId);
}


/**
 @brief 
 */
CoreClientPtr CMultiPlugDelegation::GetClientFromServerNetId(netid serverNetId)
{
	RETV(!GetMultiPlug(), NULL);	
	return GetMultiPlug()->GetClientFromServerNetId(serverNetId);
}


/**
 @brief 
 */
bool	CMultiPlugDelegation::Send(netid netId, const SEND_FLAG flag, CPacket &packet)
{
	RETV(!m_pMultiPlug, false);
	return m_pMultiPlug->Send(netId, flag, packet);
}


/**
 @brief 
 */
bool	CMultiPlugDelegation::SendAll(CPacket &packet)
{
	RETV(!m_pMultiPlug, false);
	return m_pMultiPlug->SendAll(packet);
}

