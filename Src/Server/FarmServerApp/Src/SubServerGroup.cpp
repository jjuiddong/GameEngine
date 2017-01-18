
#include "stdafx.h"
#include "SubServerGroup.h"
#include "RemoteSubServer.h"

using namespace network;

/**
 @brief clientSvrType 이 serverSvrType 서버의 클라이언트로 접속할 때, serverSvrType 의 ip 와 port 정보를 
 v 에 저장해 리턴한다.
 serverSvrType 타입의 서버가 bind 중일 때만 저장한다.
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
@brief 선택된 그룹에 Client로 접속하는 서버타입을 리턴한다.
            output_link, p2pS 에 등록된 서버타입을 리턴한다.

			svrTypes 의 기존 정보는 모두 초기화 된다.
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

	// 중복된 스트링 제거
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
