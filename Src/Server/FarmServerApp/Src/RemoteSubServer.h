/**
Name:   RemoteSubServer.h
Author:  jjuiddong
Date:    4/7/2013

서브 서버 정보를 표현한다.
*/
#pragma once

#include "LinkState.h"

/// Remote Sub Server class 
class CRemoteSubServer : public network::CSession
{
public:
	CRemoteSubServer();

	typedef std::vector<SLinkState> Links;

	void		SetP2PCLink( const std::vector<std::string> &v );	
	void		SetP2PSLink( const std::vector<std::string> &v );	
	void		SetInputLink( const std::vector<std::string> &v );
	void		SetOutputLink( const std::vector<std::string> &v );
	void		GetP2PCLink( OUT std::vector<std::string> &v );	
	void		GetP2PSLink( OUT std::vector<std::string> &v );	
	void		GetInputLink( OUT std::vector<std::string> &v );
	void		GetOutputLink( OUT std::vector<std::string> &v );

	void		GetServerInfoCorrespondClientLink( const std::string &linkSvrType, OUT std::vector<network::SHostInfo> &v );

	int		GetToBindInnerPort(int basePort);
	int		GetToBindOuterPort(int basePort);

	void		SetInnerBindPort( const std::string &linkSvrType, int bindPort );
	void		SetOuterBindPort( const std::string &linkSvrType, int bindPort );

	bool		SetBindComplete( const std::string &linkSvrType );
	bool		SetConnectComplete( const std::string &linkSvrType );

protected:
	void		GetLinkInfo( const Links &linkServers,  const std::string &linkSvrType, OUT std::vector<network::SHostInfo> &v );
	void		GetLinkSvrType( const Links &linkServers,  OUT std::vector<std::string> &v );
	bool		IsBindPort( const Links &linkServers, int port );
	SLinkState* GetLinkState( network::SERVICE_TYPE type, const std::string &linkSvrType );
	SLinkState* GetLinkState( Links &links, network::SERVICE_TYPE type, const std::string &linkSvrType );

private:
	int m_OuterPort;
	Links m_InputLink;
	Links m_OutputLink;
	Links m_P2PCLink;
	Links m_P2PSLink;

};



/// CRemoteSubServer class Factory
class CRemoteSubServerFactory : public network::ISessionFactory
{
	virtual network::CSession* New() { return new CRemoteSubServer(); }
	virtual network::ISessionFactory* Clone() { return new CRemoteSubServerFactory(); }
};
