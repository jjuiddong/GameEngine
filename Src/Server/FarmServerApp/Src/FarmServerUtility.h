/**
Name:   FarmServerUtility.h
Author:  jjuiddong
Date:    4/8/2013

*/
#pragma once

class CFarmServer;
namespace farmsvr 
{

	bool		ReadServerGrouprConfig( const std::string &fileName, CFarmServer &farmSvr);

}

