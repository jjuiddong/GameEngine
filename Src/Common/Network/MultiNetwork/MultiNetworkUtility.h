/**
Name:   MultiNetworkUtillity.h
Author:  jjuiddong
Date:    4/2/2013

*/
#pragma once

namespace network { namespace multinetwork {

	struct SSvrConfigData
	{
		std::string svrType;
		int port;
		std::string parentSvrIp;
		int parentSvrPort;
		std::vector<std::string> p2pC;
		std::vector<std::string> p2pS;
		std::vector<std::string> inputLink;
		std::vector<std::string> outputLink;

		void clear() {
			p2pC.clear();
			p2pS.clear();
			inputLink.clear();
			outputLink.clear();
		}
	};

	// Function
	bool ReadServerConfigFile( const std::string &fileName, OUT SSvrConfigData &config );

}}
