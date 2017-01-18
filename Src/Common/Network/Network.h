
#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "NetworkDef.h"
#include "Service/Server.h"
#include "Service/Client.h"
#include "Service/SubServerPlug.h"

namespace network
{
	using namespace marshalling;

	class CServerBasic;
	class CClient;

	// Server
	bool		StartServer(int port, ServerBasicPtr pSvr);
	bool		StopServer(ServerBasicPtr pSvr);
	ServerBasicPtr	GetServer(netid serverId);

	// Client
	bool		StartClient(const std::string &ip, int port, ClientBasicPtr pClt);
	bool		StopClient(ClientBasicPtr pClt);
	ClientBasicPtr	GetClient(netid clientId);
	CoreClientPtr	GetCoreClient(netid clientId);

	// Multi Network
	bool		StartMultiNetwork();
	bool		ConnectDelegation( const std::string &linkSvrType, MultiPlugDelegationPtr ptr);
	bool		AddDelegation( const std::string &linkSvrType, multinetwork::CMultiPlugDelegation *ptr);

	// Common
	bool		Init(int logicThreadCount, const std::string &svrConfigFileName="");
	void		Proc();
	void		Clear();

	// Debug
	std::string	ToString();
	std::string	GetLastError();

};

#endif // __NETWORK_H__
