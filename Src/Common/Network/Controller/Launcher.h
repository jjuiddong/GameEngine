/**
Name:    NetLauncher.h
Author:  jjuiddong
Date:    12/24/2012

*/
#pragma once

namespace network { namespace launcher {

	class CServerBasic;
	class CClient;
	class CCoreClient;

	bool	LaunchServer(ServerBasicPtr pSvr, int port);
	bool	LaunchClient(ClientPtr pClient, const std::string &ip, int port);
	bool	LaunchCoreClient(CoreClientPtr pClient, const std::string &ip, int port);
}}
