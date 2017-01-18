
#include "stdafx.h"
#include "MultiNetworkUtility.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace network { namespace multinetwork {

	void ReadChildStringType( boost::property_tree::ptree &props, const std::string &key, std::vector<std::string> &v );


}}

using namespace network;
using namespace network::multinetwork;


/**
 @brief read server config json file
 json config file field~
 type
 parent server ip
 parent server port
 p2pC
 p2pS
 input link
 output link
 */
bool multinetwork::ReadServerConfigFile( const std::string &fileName, OUT SSvrConfigData &config  )
{
	bool result = true;
	try
	{
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(fileName.c_str(), props);

		config.svrType = props.get<string>("type");
		config.parentSvrIp = props.get<string>("parent server ip", "");
		string port = props.get<string>("parent server port","");
		config.parentSvrPort = atoi(port.c_str());
		port = props.get<string>("port","");
		config.port = atoi(port.c_str());

		ReadChildStringType(props, "p2pC", config.p2pC);
		ReadChildStringType(props, "p2pS", config.p2pS);
		ReadChildStringType(props, "input link", config.inputLink);
		ReadChildStringType(props, "output link", config.outputLink);
	}
	catch (std::exception &e)
	{
		clog::Error( clog::ERROR_CRITICAL, 0, "ReadServerConfigFile() Error!! [%s]",
			e.what());
		return false;
	}

	return result;
}


/**
 @brief read property_tree child item and then save vector container
 */
void multinetwork::ReadChildStringType( boost::property_tree::ptree &props, 
	const std::string &key, std::vector<std::string> &v )
{
	try
	{
		using boost::property_tree::ptree;
		ptree::assoc_iterator itor = props.find(key);
		if (props.not_found() != itor)
		{
			BOOST_FOREACH(auto &child, itor->second)
				v.push_back(child.second.data());				
		}
	}
	catch (...)
	{
	}
}
