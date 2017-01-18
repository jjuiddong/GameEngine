
#include "stdafx.h"
#include "MultiNetwork.h"
#include "MultiNetworkUtility.h"
#include "MultiPlug.h"
#include "MultiPlugDelegation.h"
#include "FarmServerPlug.h"


using namespace network;
using namespace network::multinetwork;


CMultiNetwork::CMultiNetwork() :
	CPlug(SERVICE_SEPERATE_THREAD)
,	m_pFarmSvrConnector(NULL)
{
	m_Config.clear();

}

CMultiNetwork::~CMultiNetwork()
{
	Cleanup();
}


/**
 @brief 
 */
bool	CMultiNetwork::Init( const std::string &configFileName )
{
	if (!ReadServerConfigFile(configFileName, m_Config))
		return false;

	if ("farmsvr" == m_Config.svrType)
	{
		CMultiPlug *pMainCtrl = new CMultiPlug(SERVER, m_Config.svrType, "client");
		if (!AddController(pMainCtrl))
			return false;
	}
	else
	{
		// create farmsvr connector, and delegation
		if (!m_Config.parentSvrIp.empty())
		{
			CMultiPlug *pCtrl = new CMultiPlug(CLIENT, m_Config.svrType, "farmsvr");
			if (!AddController(pCtrl))
				return false;

			m_pFarmSvrConnector = new CFarmServerPlug( m_Config.svrType, m_Config );
			if (!ConnectDelegation("farmsvr", m_pFarmSvrConnector))
				return false;
		}
		else
		{
			// must fill parent server ip, port field
			return false;
		}
	}

	return true;
}


/**
 @brief 
 */
void	CMultiNetwork::Cleanup()
{
	BOOST_FOREACH(auto &ctrl, m_Controllers.m_Seq)
	{
		SAFE_DELETE(ctrl);
	}
	m_Controllers.clear();

	SAFE_DELETE( m_pFarmSvrConnector );

	BOOST_FOREACH(auto &ctrl, m_Delegations.m_Seq)
	{
		SAFE_DELETE(ctrl);
	}
	m_Delegations.clear();

}


/**
 @brief ConnectDelegation
 */
bool	CMultiNetwork::ConnectDelegation( const std::string &linkSvrType, MultiPlugDelegationPtr ptr)
{
	RETV(!ptr, false);

	auto it = m_Controllers.find( linkSvrType );
	if (m_Controllers.end() == it)
		return false; // not exist

	ptr->AddChild( it->second );
	ptr->SetMultiPlug( it->second );
	return true;
}


/**
 @brief 
 */
bool	CMultiNetwork::Start()
{
	if ("farmsvr" == m_Config.svrType)
	{
		MultiPlugPtr ptr  = GetMultiPlug("client");
		if (!ptr) return false;
		ptr->Start("localhost", m_Config.port);
	}
	else
	{
		if (m_pFarmSvrConnector)
			m_pFarmSvrConnector->Start( m_Config.parentSvrIp, m_Config.parentSvrPort );
	}
	return true;
}


/**
 @brief 
 */
bool	CMultiNetwork::Stop()
{

	return true;
}


/**
 @brief 
 */
bool	CMultiNetwork::Proc()
{

	return true;
}


/**
 @brief Add NetGroupController object
 */
bool	CMultiNetwork::AddController( CMultiPlug *ptr )
{
	RETV(!ptr, false);

	auto it = m_Controllers.find( ptr->GetConnectSvrType() );
	if (m_Controllers.end() != it)
		return false; // already exist

	m_Controllers.insert( Controllers::value_type(ptr->GetConnectSvrType(), ptr) );
	return true;
}


/**
 @brief Remove NetGroupController Object
 */
bool	CMultiNetwork::RemoveController( const std::string &linkSvrType )
{
	auto it = m_Controllers.find( linkSvrType );
	if (m_Controllers.end() == it)
		return false; // not exist

	m_Controllers.remove( linkSvrType );
	return true;
}


/**
 @brief Get NetGroupController Object
 */
MultiPlugPtr CMultiNetwork::GetMultiPlug( const std::string &linkSvrType )
{
	auto it = m_Controllers.find( linkSvrType );
	if (m_Controllers.end() == it)
		return NULL; // not exist
	return it->second;
}


/**
 @brief Delegation 추가
 */
bool	CMultiNetwork::AddDelegation( const std::string &linkSvrType, CMultiPlugDelegation *ptr)
{
	RETV(!ptr, false);

	auto it = m_Delegations.find( linkSvrType );
	if (m_Delegations.end() != it)
		return false; // already exist

	if (!ConnectDelegation(linkSvrType, ptr))
		return false;

	m_Delegations.insert( Delegations::value_type(linkSvrType, ptr) );
	return true;
}


/**
 @brief Delegation 제거
 */
bool	CMultiNetwork::RemoveDelegation( const std::string &linkSvrType )
{
	auto it = m_Delegations.find( linkSvrType );
	if (m_Delegations.end() == it)
		return false; // not exist

	m_Delegations.remove( linkSvrType );
	return true;
}


/**
 @brief GetDelegation
 */
MultiPlugDelegationPtr CMultiNetwork::GetDelegation( const std::string &linkSvrType )
{
	auto it = m_Delegations.find( linkSvrType );
	if (m_Delegations.end() == it)
		return NULL; // not exist
	return it->second;
}


/**
 @brief 
 */
bool	CMultiNetwork::Send(netid netId, const SEND_FLAG flag, CPacket &packet)
{
	BOOST_FOREACH( auto ctrl, m_Controllers.m_Seq)
	{
		ctrl->Send(netId, flag, packet);
	}
	return true;
}


/**
 @brief 
 */
bool	CMultiNetwork::SendAll(CPacket &packet)
{
	BOOST_FOREACH( auto ctrl, m_Controllers.m_Seq)
	{
		ctrl->SendAll(packet);
	}
	return true;
}


/**
 @brief 
 */
MultiPlugPtr	CMultiNetwork::GetControllerFromNetId( netid netId )
{
	BOOST_FOREACH( auto ctrl, m_Controllers.m_Seq)
	{
		if (ctrl->GetSession(netId))
			return ctrl;
	}
	return NULL;
}
