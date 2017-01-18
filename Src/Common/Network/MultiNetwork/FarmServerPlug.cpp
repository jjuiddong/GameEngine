
#include "stdafx.h"
#include "FarmServerPlug.h"
#include "MultiNetwork.h"
#include "Network/Controller/CoreClient.h"

#include "NetProtocol/Src/farm_Protocol.cpp"
#include "NetProtocol/Src/farm_ProtocolListener.cpp"

using namespace network;
using namespace network::multinetwork;


CFarmServerPlug::CFarmServerPlug(const std::string &svrType, const SSvrConfigData &config) :
	m_IsDetectedSendConfig(false)
{
	m_Config = config;
	CreateLink();
}


/**
@brief OnConnectNetGroupController
*/
void	CFarmServerPlug::OnConnectMultiPlug()
{
	NETEVENT_CONNECT(EVT_CONNECT, CFarmServerPlug, CFarmServerPlug::OnConnect );
	AddProtocolListener(this);
	RegisterProtocol( &m_Protocol );
}


/**
 @brief 
 */
bool CFarmServerPlug::Start( const std::string &ip, const int port )
{
	if (!GetMultiPlug())
		return false;

	GetMultiPlug()->Start(ip, port);
	return true;
}


/**
 @brief P2P, Input_lin, Output_link 按眉甫 积己茄促.
 */
void	CFarmServerPlug::CreateLink()
{
	// Input Link 积己
	BOOST_FOREACH(auto &bindSubSvrType, m_Config.inputLink)
	{
		CreateSubController( CLIENT, true, m_Config.svrType, bindSubSvrType );
	}

	// Output Link 积己
	BOOST_FOREACH(auto &bindSubSvrType, m_Config.outputLink)
	{
		CreateSubController( SERVER, true, m_Config.svrType, bindSubSvrType );
	}

	// P2P Client Link 积己
	BOOST_FOREACH(auto &bindSubSvrType, m_Config.p2pC)
	{
		CreateSubController( CLIENT, true, m_Config.svrType, bindSubSvrType );
	}

	// P2P Server Link 积己
	if (m_Config.p2pS.size() > 0)
	{
		CreateSubController( SERVER, true, m_Config.svrType,  "p2p" );
	}	

	// 扁夯 辑滚 积己 
	CreateSubController( SERVER, false, m_Config.svrType,  "client" );
}


/**
 @brief ConnectLink
 */
void	CFarmServerPlug::ConnectLink()
{
	// Input Link 积己
	BOOST_FOREACH(auto &bindSubSvrType, m_Config.inputLink)
	{
		ConnectSubController( CLIENT, true, m_Config.svrType, bindSubSvrType );
	}

	// Output Link 积己
	BOOST_FOREACH(auto &bindSubSvrType, m_Config.outputLink)
	{
		ConnectSubController( SERVER, true, m_Config.svrType, bindSubSvrType );
	}

	// P2P Client Link 积己
	BOOST_FOREACH(auto &bindSubSvrType, m_Config.p2pC)
	{
		ConnectSubController( CLIENT, true, m_Config.svrType, bindSubSvrType );
	}

	// P2P Server Link 积己
	if (m_Config.p2pS.size() > 0)
	{
		ConnectSubController( SERVER, true, m_Config.svrType,  "p2p" );
	}

	// 扁夯 辑滚 Bind
	ConnectSubController( SERVER, false, m_Config.svrType,  "client" );
}


/**
 @brief NetGroupController 积己
 @param IsInnerBind : true: to Bind Inner Space Network
									 false: to Bind Outer Space Network
 */
bool	CFarmServerPlug::CreateSubController( SERVICE_TYPE serviceType, bool IsInnerBind,
	const std::string &connectSubSvrType, const std::string &bindSubSvrType )
{
	if (connectSubSvrType == bindSubSvrType)
		return false;

	MultiPlugPtr ptr = CMultiNetwork::Get()->GetMultiPlug(bindSubSvrType);
	if (ptr)
		return true;

	CMultiPlug *pctrl = new CMultiPlug(serviceType, connectSubSvrType, bindSubSvrType);
	if (!CMultiNetwork::Get()->AddController(pctrl))
	{
		clog::Error(log::ERROR_CRITICAL, "Not Create NetGroupController !!" );
		delete pctrl;
		return false;
	}

	NETEVENT_CONNECT_TO( pctrl, this, EVT_CONNECT, CFarmServerPlug, CFarmServerPlug::OnConnectLink );
	NETEVENT_CONNECT_TO( pctrl, this, EVT_LISTEN, CFarmServerPlug, CFarmServerPlug::OnConnectLink );
	NETEVENT_CONNECT_TO( pctrl, this, EVT_DISCONNECT, CFarmServerPlug, CFarmServerPlug::OnDisconnectLink );
	return true;
}


/**
@brief ConnectSubController
*/
void	CFarmServerPlug::ConnectSubController( SERVICE_TYPE serviceType, bool IsInnerBind,
	const std::string &connectSubSvrType, const std::string &bindSubSvrType )
{
	MultiPlugPtr ptr = CMultiNetwork::Get()->GetMultiPlug(bindSubSvrType);
	if (!ptr)
		return;

	if (ptr->IsConnect())
		return;

	ptr->SetTryConnect();
	if (CLIENT == serviceType)
	{
		m_Protocol.ReqServerInfoList( SERVER_NETID, SEND_T, connectSubSvrType,
			bindSubSvrType );
	}
	else if (SERVER == serviceType)
	{
		if (IsInnerBind)
			m_Protocol.ReqToBindInnerPort( SERVER_NETID, SEND_T, bindSubSvrType );
		else
			m_Protocol.ReqToBindOuterPort( SERVER_NETID, SEND_T, bindSubSvrType );
	}
}


/**
 @brief Login 夸没
 */
void CFarmServerPlug::OnConnect(CNetEvent &event)
{
	m_Protocol.ReqSubServerLogin( SERVER_NETID, SEND_T, m_Config.svrType );
}


/**
@brief OnConnectLink
 */
void CFarmServerPlug::OnConnectLink(CNetEvent &event)
{
	CMultiPlug *pctrl = dynamic_cast<CMultiPlug*>(event.GetEventObject().Get());
	if (!pctrl)
		return;

	if (pctrl->GetServiceType() == CLIENT)
		m_Protocol.ReqSubClientConnectComplete( SERVER_NETID, SEND_T, pctrl->GetConnectSvrType() );
	else if (pctrl->GetServiceType() == SERVER)
		m_Protocol.ReqSubServerBindComplete( SERVER_NETID, SEND_T, pctrl->GetConnectSvrType() );
}


/**
@brief OnDisconnectLink
 */
void CFarmServerPlug::OnDisconnectLink(CNetEvent &event)
{
	//clog::ErrorMsg( "Err!!! Not Connect or Not Bind NetGroupController" );
}


/**
 @brief 
 */
bool CFarmServerPlug::AckSubServerLogin(farm::AckSubServerLogin_Packet &packet)
{
	if (packet.errorCode == error::ERR_SUCCESS)
	{
		m_Protocol.SendSubServerP2PCLink( SERVER_NETID, SEND_T, m_Config.p2pC );
		m_Protocol.SendSubServerP2PSLink( SERVER_NETID, SEND_T, m_Config.p2pS );
		m_Protocol.SendSubServerInputLink( SERVER_NETID, SEND_T, m_Config.inputLink );
		m_Protocol.SendSubServerOutputLink( SERVER_NETID, SEND_T, m_Config.outputLink );
		return true;
	}
	else
	{
		clog::Error( clog::ERROR_CRITICAL, "AckSubServerLogin Error!! errorCode= %d, svrType = %s", packet.errorCode, m_Config.svrType.c_str() );
		clog::ErrorMsg( common::format( "AckSubServerLogin Error!! errorCode= %d, svrType = %s", packet.errorCode, m_Config.svrType.c_str()) );
		return false;
	}
}


/**
 @brief AckSendSubServerP2PCLink
 */
bool CFarmServerPlug::AckSendSubServerP2PCLink(farm::AckSendSubServerP2PCLink_Packet &packet)
{
	if (!m_IsDetectedSendConfig)
		m_IsDetectedSendConfig = (packet.errorCode != error::ERR_SUCCESS);

	if (packet.errorCode != error::ERR_SUCCESS)
		clog::Error( clog::ERROR_CRITICAL, "P2PC Link Error!!" );

	return true;
}


/**
 @brief AckSendSubServerP2PSLink
 */
bool CFarmServerPlug::AckSendSubServerP2PSLink(farm::AckSendSubServerP2PSLink_Packet &packet)
{
	if (!m_IsDetectedSendConfig)
		m_IsDetectedSendConfig = (packet.errorCode != error::ERR_SUCCESS);

	if (packet.errorCode != error::ERR_SUCCESS)
		clog::Error( clog::ERROR_CRITICAL, "P2PS Link Error!!" );

	return true;
}

/**
 @brief 
 */
bool CFarmServerPlug::AckSendSubServerInputLink(farm::AckSendSubServerInputLink_Packet &packet)
{
	if (!m_IsDetectedSendConfig)
		m_IsDetectedSendConfig = (packet.errorCode != error::ERR_SUCCESS);

	if (packet.errorCode != error::ERR_SUCCESS)
		clog::Error( clog::ERROR_CRITICAL, "P2P Link Error!!" );

	return true;
}


/**
 @brief 
 */
bool CFarmServerPlug::AckSendSubServerOutputLink(farm::AckSendSubServerOutputLink_Packet &packet)
{
	if (!m_IsDetectedSendConfig)
		m_IsDetectedSendConfig = (packet.errorCode != error::ERR_SUCCESS);

	if (m_IsDetectedSendConfig)
	{
		clog::Error( clog::ERROR_CRITICAL, "AckSendSubServerOutputLink Error"  );
		return false;
	}

	ConnectLink();
	return true;
}


/**
 @brief 
 */
bool CFarmServerPlug::AckServerInfoList(farm::AckServerInfoList_Packet &packet)
{
	if (packet.errorCode != error::ERR_SUCCESS)
	{
		clog::Error( clog::ERROR_CRITICAL, 
			"AckServerInfoList Error!!, not found port number  clientSvrType = %s, serverSvrType = %s", 
			packet.clientSvrType.c_str(), packet.serverSvrType.c_str() );
		return true;
	}

	// Connect Client
	MultiPlugPtr ptr = CMultiNetwork::Get()->GetMultiPlug(packet.serverSvrType);
	if (!ptr)
	{
		clog::Error( clog::ERROR_CRITICAL, 
			"AckServerInfoList Error!!, not found controller serverSvrType = %s", 
			packet.serverSvrType.c_str() );
		return true;
	}
	
	if (!ptr->Start(packet.v))
	{
		clog::Error( clog::ERROR_CRITICAL, "NetGroupController Start Error!!" );
		return false;
	}
	return true;
}


/**
 @brief 
 */
bool CFarmServerPlug::AckToBindOuterPort(farm::AckToBindOuterPort_Packet &packet)
{
	if (packet.errorCode != error::ERR_SUCCESS)
	{
		clog::Error( clog::ERROR_CRITICAL, 
			"AckToBindOuterPort Error!!, not found port number  bindSubServerSvrType = %s", 
			packet.bindSubServerSvrType.c_str() );
		return false;
	}

	MultiPlugPtr ptr = CMultiNetwork::Get()->GetMultiPlug(packet.bindSubServerSvrType);
	if (!ptr)
	{
		clog::Error( clog::ERROR_CRITICAL, 
			"AckToBindOuterPort Error!!, not found controller bindSubServerSvrType = %s", 
			packet.bindSubServerSvrType.c_str() );
		return false;
	}

	// Server Bind
	if (!ptr->Start( "", packet.port))
	{
		clog::Error( clog::ERROR_CRITICAL, "NetGroupController Start Error!!" );
		return false;
	}
	return true;
}


/**
 @brief 
 */
bool CFarmServerPlug::AckToBindInnerPort(farm::AckToBindInnerPort_Packet &packet)
{
	if (packet.errorCode != error::ERR_SUCCESS)
	{
		clog::Error( clog::ERROR_CRITICAL, 
			"AckToBindInnerPort Error!!, not found port number  bindSubServerSvrType = %s", 
			packet.bindSubServerSvrType.c_str() );
		return false;
	}

	MultiPlugPtr ptr = CMultiNetwork::Get()->GetMultiPlug(packet.bindSubServerSvrType);
	if (!ptr)
	{
		clog::Error( clog::ERROR_CRITICAL, 
			"AckToBindInnerPort Error!!, not found controller bindSubServerSvrType = %s", 
			packet.bindSubServerSvrType.c_str() );
		return false;
	}

	// Server Bind
	if (!ptr->Start( "", packet.port))
	{
		clog::Error( clog::ERROR_CRITICAL, "NetGroupController Start Error!!" );
		return false;
	}
	return true;
}


/**
 @brief 
 */
bool CFarmServerPlug::AckSubServerBindComplete(farm::AckSubServerBindComplete_Packet &packet)
{
	
	return true;
}


/**
 @brief 
 */
bool CFarmServerPlug::AckSubClientConnectComplete(farm::AckSubClientConnectComplete_Packet &packet)
{

	return true;
}


/**
 @brief 
 */
bool CFarmServerPlug::BindSubServer(farm::BindSubServer_Packet &packet)
{
	MultiPlugPtr ptr = CMultiNetwork::Get()->GetMultiPlug(packet.bindSubSvrType);
	if (!ptr)
	{
		clog::Error( clog::ERROR_CRITICAL, "BindSubServer Error!! not found bindSubSvr : %s", 
			packet.bindSubSvrType.c_str() );
		return false;
	}

	if (!ptr->Start(packet.ip, packet.port))
	{
		clog::Error( clog::ERROR_CRITICAL, "BindSubServer Start Error!! ip=%s, port=%d", 
			packet.ip.c_str(), packet.port );
		return false;
	}
	return true;
}
