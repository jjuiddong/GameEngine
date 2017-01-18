
#include "stdafx.h"
#include "SubServerPlug.h"

using namespace network;


CSubServerPlug::CSubServerPlug(const std::string &appSvrType) :
	m_AppSvrType(appSvrType)
{

}

CSubServerPlug::~CSubServerPlug()
{
	BOOST_FOREACH(auto &svr, m_RemoteServers.m_Seq)
	{
		SAFE_DELETE(svr);
	}
	m_RemoteServers.clear();
}


/**
 @brief OnConnectNetGroupController
 */
void	CSubServerPlug::OnConnectMultiPlug()
{
	multinetwork::CMultiPlugDelegation::OnConnectMultiPlug();

	AddProtocolListener( this );
	GetMultiPlug()->SetSessionFactory( new CRemoteServerFactory() );
	RegisterProtocol( &m_ServerNetwork_Protocol );

	NETEVENT_CONNECT(EVT_CONNECT, CSubServerPlug, CSubServerPlug::OnConnectSubLink);
	NETEVENT_CONNECT(EVT_CLIENT_JOIN, CSubServerPlug, CSubServerPlug::OnConnectSubLink);
}


/**
 @brief  Call, client connection, or server attach remote client
 */
void	CSubServerPlug::OnConnectSubLink(CNetEvent &event )
{
	m_ServerNetwork_Protocol.SendServerInfo( event.GetNetId(), SEND_T, 
		m_AppSvrType, "localhost", 0, 0 );
}


/**
 @brief ���� ������κ��� ���� ���� ������ ������Ʈ �޴´�.
 */
bool CSubServerPlug::SendServerInfo(server_network::SendServerInfo_Packet &packet)
{
	auto it = m_RemoteServers.find(packet.senderId);
	if (m_RemoteServers.end() == it)
	{
		CRemoteServer *pNewSvr = new CRemoteServer();
		pNewSvr->SetNetId(packet.senderId);
		pNewSvr->SetIp( packet.ip );
		pNewSvr->SetPort(packet.port);
		pNewSvr->SetNetId(packet.senderId);
		pNewSvr->SetUserCount(packet.userCount);
		m_RemoteServers.insert( RemoteServers::value_type(packet.senderId, pNewSvr) );
	}
	else
	{
		it->second->SetIp( packet.ip );
		it->second->SetPort(packet.port);
		it->second->SetUserCount(packet.userCount);
	}

	return true;
}


/**
 @brief GetSubServerInfo
 */
std::list<SSubServerInfo> CSubServerPlug::GetSubServerInfo()
{
	std::list<SSubServerInfo> servers;
	MultiPlugPtr pCtrl = GetMultiPlug();
	RETV(!pCtrl, servers);

	if (SERVER == pCtrl->GetServiceType()) // server
	{
		RETV(!GetServer(), servers);
		BOOST_FOREACH(auto &svr, m_RemoteServers.m_Seq)
		{
			// ���ӵ� Ŭ���̾�Ʈ���� Ȯ���Ѵ�. ���ٸ� ����.
			CSession *pClient = GetServer()->GetSession( svr->GetNetId() );
			if (pClient)
			{
					SSubServerInfo info;
					info.serverId = svr->GetNetId();
					info.ip = svr->GetIp();
					info.portnum = svr->GetPort();
					info.userCnt = svr->GetUserCount();
					servers.push_back( info );
			}
			else
			{
				m_RemoteServers.remove( svr->GetNetId() );
			}
		}
	}
	else // client
	{
		RETV(!GetServer(), servers);
		BOOST_FOREACH(auto &svr, m_RemoteServers.m_Seq)
		{
			// ���ӵ� Ŭ���̾�Ʈ���� Ȯ���Ѵ�. ���ٸ� ����.
			CSession *pClient = GetClientFromServerNetId( svr->GetNetId() );
			if (pClient)
			{
				SSubServerInfo info;
				info.serverId = svr->GetNetId();
				info.ip = svr->GetIp();
				info.portnum = svr->GetPort();
				info.userCnt = svr->GetUserCount();
				servers.push_back( info );				
			}
			else
			{
				m_RemoteServers.remove( svr->GetNetId() );
			}
		}
	}

	m_RemoteServers.apply_removes();

	return  servers;
}


/**
@brief  serverId  ��  Server ������ �����Ѵ�.
*/
SSubServerInfo CSubServerPlug::GetSubServerInfo(netid serverId)
{
	SSubServerInfo info;
	info.serverId = INVALID_NETID;

	std::list<SSubServerInfo> subServers = GetSubServerInfo();
	BOOST_FOREACH(auto &svr, subServers)
	{
		if (serverId == svr.serverId)
			return svr;
	}

	return info;
}
