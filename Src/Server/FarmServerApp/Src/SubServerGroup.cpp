
#include "stdafx.h"
#include "SubServerGroup.h"
#include "RemoteSubServer.h"

using namespace network;

/**
 @brief clientSvrType �� serverSvrType ������ Ŭ���̾�Ʈ�� ������ ��, serverSvrType �� ip �� port ������ 
 v �� ������ �����Ѵ�.
 serverSvrType Ÿ���� ������ bind ���� ���� �����Ѵ�.
 */
bool CSubServerGroup::GetServerInfoCorrespondClient( const std::string &clientSvrType, const std::string &serverSvrType, 
	ISessionAccess &userAccess, OUT std::vector<SHostInfo> &v)
{
	BOOST_FOREACH(auto &netId, GetPlayers())
	{
		RemoteSubServerPtr pSubServer = dynamic_cast<CRemoteSubServer*>(
			userAccess.GetSession(netId).Get());
		if (!pSubServer)
			continue;
		pSubServer->GetServerInfoCorrespondClientLink( clientSvrType, v );
	}
	return true;
}


/**
@brief ���õ� �׷쿡 Client�� �����ϴ� ����Ÿ���� �����Ѵ�.
            output_link, p2pS �� ��ϵ� ����Ÿ���� �����Ѵ�.

			svrTypes �� ���� ������ ��� �ʱ�ȭ �ȴ�.
*/
bool CSubServerGroup::GetCorrespondClientInfo( ISessionAccess &userAccess, OUT std::vector<std::string> &svrTypes )
{
	svrTypes.clear();
	svrTypes.reserve(32);

	BOOST_FOREACH(auto &netId, GetPlayers())
	{
		RemoteSubServerPtr pSubServer = dynamic_cast<CRemoteSubServer*>(
			userAccess.GetSession(netId).Get());
		if (!pSubServer)
			continue;

		pSubServer->GetOutputLink( svrTypes );
		pSubServer->GetP2PSLink( svrTypes );
	}

	// �ߺ��� ��Ʈ�� ����
	auto it = std::unique(svrTypes.begin(), svrTypes.end());
	svrTypes.erase(it, svrTypes.end());
	return true;
}


/**
 @brief GetToBindOuterPort
 */
int CSubServerGroup::GetToBindOuterPort(ISessionAccess &userAccess)
{
	int port = m_OuterPortBase;
	BOOST_FOREACH(auto &netId, GetPlayers())
	{
		RemoteSubServerPtr pSubServer = dynamic_cast<CRemoteSubServer*>(
			userAccess.GetSession(netId).Get());
		if (!pSubServer)
			continue;
		port = pSubServer->GetToBindOuterPort(port);
	}
	return port;
}


/**
 @brief GetToBindInnerPort
 */
int CSubServerGroup::GetToBindInnerPort(ISessionAccess &userAccess)
{
	int port = m_InnerPortBase;
	BOOST_FOREACH(auto &netId, GetPlayers())
	{
		RemoteSubServerPtr pSubServer = dynamic_cast<CRemoteSubServer*>(
			userAccess.GetSession(netId).Get());
		if (!pSubServer)
			continue;
		port = pSubServer->GetToBindInnerPort(port);
	}
	return port;
}
