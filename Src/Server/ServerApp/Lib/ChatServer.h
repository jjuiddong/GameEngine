//------------------------------------------------------------------------
// Name:    ChatServer.h
// Author:  jjuiddong
// Date:    12/21/2012
// 
// 테스트 코드
// 채팅서버
//------------------------------------------------------------------------

#pragma once

#include "NetProtocol/Src/basic_Protocol.h"
#include "NetProtocol/Src/basic_ProtocolListener.h"

DECLARE_TYPE_NAME(CChatServer)
class CChatServer : public network::CServer
								,public memmonitor::Monitor<CChatServer, TYPE_NAME(CChatServer)>
{
public:
	CChatServer()
	{
		RegisterProtocol(&m_S2CProtocol);
		//AddListener( this );
	}

protected:
	basic::s2c_Protocol m_S2CProtocol;

public:
	//virtual void func2(netid senderId, const std::string &str) override
	//{
	//	//Stop(); Test Code
	//}

// 	virtual void func2(netid senderId, const std::string &str) override
// 	{
// 		std::string sndStr = "server send ";
// 		sndStr += str;
// 		m_S2CProtocol.func2(senderId, sndStr);
// 	}
// 	virtual void func3(netid senderId, const float &value) override
// 	{
// 		int a = 0;
// 	}

};
