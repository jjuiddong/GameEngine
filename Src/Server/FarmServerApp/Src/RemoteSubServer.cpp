
#include "stdafx.h"
#include "RemoteSubServer.h"

using namespace network;

CRemoteSubServer::CRemoteSubServer() : 
	m_OuterPort(0)
{

}


/**
 @brief SetP2PLink
 */
void CRemoteSubServer::SetP2PCLink( const std::vector<std::string> &v )
{
	m_P2PCLink.clear();
	BOOST_FOREACH(auto &svrType, v)
	{
		SLinkState link;
		link.m_SvrType = svrType;
		link.m_State = SLinkState::WAIT;
		link.m_Type = network::CLIENT;
		m_P2PCLink.push_back( link );
	}
}


/**
 @brief SetP2PLink
 */
void CRemoteSubServer::SetP2PSLink( const std::vector<std::string> &v )
{
	m_P2PSLink.clear();
	BOOST_FOREACH(auto &svrType, v)
	{
		SLinkState link;
		link.m_SvrType = svrType;
		link.m_State = SLinkState::WAIT;
		link.m_Type = network::SERVER;
		m_P2PSLink.push_back( link );
	}
}


/**
 @brief SetInputLink
 */
void	CRemoteSubServer::SetInputLink( const std::vector<std::string> &v )
{
	m_InputLink.clear();
	BOOST_FOREACH(auto &svrType, v)
	{
		SLinkState link;
		link.m_SvrType = svrType;
		link.m_State = SLinkState::WAIT;
		link.m_Type = network::CLIENT;
		m_InputLink.push_back( link );
	}
}


/**
 @brief SetOutputLink
 */
void	CRemoteSubServer::SetOutputLink( const std::vector<std::string> &v )
{
	m_OutputLink.clear();
	BOOST_FOREACH(auto &svrType, v)
	{
		SLinkState link;
		link.m_SvrType = svrType;
		link.m_State = SLinkState::WAIT;
		link.m_Type = network::SERVER;
		m_OutputLink.push_back( link );
	}
}


/**
 @brief GetClientLinkInfo
 �ش� ������ Ŭ���̾�Ʈ�� �����ϴ� �������� bind �� ������ ip, port�� �Ѱ��ش�.		
 */
void	CRemoteSubServer::GetServerInfoCorrespondClientLink( const std::string &linkSvrType, OUT std::vector<SHostInfo> &v )
{
	if (linkSvrType == "p2p")
	{
		if (!m_P2PSLink.empty())
		{
			v.push_back( 
				SHostInfo(m_P2PSLink.front().m_ServerIp, m_P2PSLink.front().m_ServerPort) );
		}
	}
	else
	{
		GetLinkInfo( m_OutputLink, linkSvrType, v );
		GetLinkInfo( m_P2PSLink, linkSvrType, v );
	}
}


/**
 @brief GetLinkInfo
 linkSvrType �� �����ϴ� Ŭ���̾�Ʈ�� �ִٸ� bind ���� ������ ip�� port������ �Ѱ��ش�.
 */
void	CRemoteSubServer::GetLinkInfo( const Links &linkServers,  const std::string &linkSvrType, OUT std::vector<SHostInfo> &v )
{
	BOOST_FOREACH(auto &link, linkServers)
	{
		if (link.m_SvrType == linkSvrType &&
			link.m_State == SLinkState::CONNECT )
			//link.m_Type == SERVER) p2p �� ���� server/client�� ������ �ʿ� ���� (���� ����)
		{
			SHostInfo info;
			info.ip = link.m_ServerIp;
			info.portnum = link.m_ServerPort;
			v.push_back(info);
		}
	}
}


/**
 @brief GetToBindInnerPort
  ���ڷ� �Ѿ�� port �� �̹� Bind ���̸� true �� �����ϰ�, �׷��� �ʴٸ�
  false�� �����Ѵ�.
 */
bool CRemoteSubServer::IsBindPort(const Links &linkServers, int port)
{
	BOOST_FOREACH(auto &link, linkServers)
	{
		if (link.m_Type == SERVER &&
			link.m_ServerPort == port)
		{
			return true;
		}
	}
	return false;
}


/**
 @brief GetToBindInnerPort
 */
int CRemoteSubServer::GetToBindInnerPort(int basePort)
{
	int bindPort = basePort;
	for (; bindPort < basePort+100; ++bindPort)
	{
		if (IsBindPort( m_OutputLink, bindPort))
			continue;
		if (IsBindPort( m_P2PSLink, bindPort))
			continue;
		break;
	}
	return bindPort;
}


/**
 @brief GetToBindOuterPort
 ���� outerport ���� �ϳ��� �����ϰ� �ȴ�.
 */
int CRemoteSubServer::GetToBindOuterPort(int basePort)
{
	if (m_OuterPort == basePort)
		return m_OuterPort + 1;
	return basePort;
}


/**
 @brief SetInnerBindPort
 */
void	CRemoteSubServer::SetInnerBindPort( const std::string &linkSvrType, int bindPort )
{
	if (linkSvrType == "p2p")
	{
		BOOST_FOREACH(auto &link, m_P2PSLink)
		{
			link.m_ServerIp = GetIp();
			link.m_ServerPort = bindPort;	
		}
	}
	else
	{
		SLinkState *p = GetLinkState(SERVER, linkSvrType);
		if (!p)
			return;

		p->m_ServerIp = GetIp();
		p->m_ServerPort = bindPort;	
	}
}


/**
 @brief SetOuterBindPort
 */
void	CRemoteSubServer::SetOuterBindPort( const std::string &linkSvrType, int bindPort )
{
	m_OuterPort = bindPort;
}


/**
 @brief GetLinkState
 */
SLinkState* CRemoteSubServer::GetLinkState( SERVICE_TYPE type, const std::string &linkSvrType )
{
	SLinkState *pRet = GetLinkState( m_InputLink, type, linkSvrType );
	if (pRet) return pRet;

	pRet = GetLinkState( m_OutputLink, type, linkSvrType );
	if (pRet) return pRet;

	pRet = GetLinkState( m_P2PCLink, type, linkSvrType );
	if (pRet) return pRet;

	if ((linkSvrType == "p2p") && !m_P2PSLink.empty())
		return &m_P2PSLink.front();		

	pRet = GetLinkState( m_P2PSLink, type, linkSvrType );
	if (pRet) return pRet;

	return NULL;
}


/**
 @brief GetLinkState
 */
SLinkState* CRemoteSubServer::GetLinkState( Links &links, SERVICE_TYPE type, const std::string &linkSvrType )
{
	BOOST_FOREACH(auto &svr, links)
	{
		if ((svr.m_Type == type) &&
			(svr.m_SvrType == linkSvrType))
		{
			return &svr;
		}
	}
	return NULL;
}


/**
 @brief SetBindComplete
 */
bool	CRemoteSubServer::SetBindComplete( const std::string &linkSvrType )
{
	if (linkSvrType == "p2p")
	{
		BOOST_FOREACH(auto &link, m_P2PSLink)
		{
			link.m_State = SLinkState::CONNECT;
		}
	}
	else
	{
		SLinkState *pLink = GetLinkState( m_OutputLink, SERVER, linkSvrType );
		if (!pLink)
			pLink = GetLinkState( m_P2PSLink, SERVER, linkSvrType );
		if (!pLink)
			return false;
		pLink->m_State = SLinkState::CONNECT;
	}
	return true;
}


/**
 @brief SetConnectComplete
 */
bool CRemoteSubServer::SetConnectComplete( const std::string &linkSvrType )
{
	SLinkState *pLink = GetLinkState( m_InputLink, CLIENT, linkSvrType );
	if (!pLink)
		pLink = GetLinkState( m_P2PCLink, CLIENT, linkSvrType );
	if (!pLink)
		return false;
	pLink->m_State = SLinkState::CONNECT;
	return true;
}


/**
 @brief GetP2PCLink
 */
void CRemoteSubServer::GetP2PCLink(OUT std::vector<std::string>& v)
{
	GetLinkSvrType(m_P2PCLink, v);
}


/**
 @brief GetP2PSLink
 */
void CRemoteSubServer::GetP2PSLink(OUT std::vector<std::string>& v)
{
	GetLinkSvrType(m_P2PSLink, v);
}


/**
 @brief GetInputLink
 */
void CRemoteSubServer::GetInputLink(OUT  std::vector<std::string>& v)
{
	GetLinkSvrType(m_InputLink, v);
}


/**
 @brief GetOutputLink
 */
void CRemoteSubServer::GetOutputLink(OUT std::vector<std::string>& v)
{
	GetLinkSvrType(m_OutputLink, v);
}


/**
 @brief Link �� SvrType �� �����Ѵ�.
 */
void	CRemoteSubServer::GetLinkSvrType( const Links &linkServers,  OUT std::vector<std::string> &v )
{
	BOOST_FOREACH(auto &link, linkServers)
	{
		v.push_back(link.m_SvrType);
	}
}
