/**
Name:   LinkState.h
Author:  jjuiddong
Date:    4/8/2013

*/
#pragma once

/// P2P link, Input, output Link Class	
class SLinkState
{
public:
	enum STATE
	{
		WAIT,
		CONNECT,
	};

	SLinkState() {}
	SLinkState(const SLinkState &rhs);
	virtual ~SLinkState() {}

//private:
	std::string m_SvrType;
	STATE m_State;
	network::SERVICE_TYPE m_Type;
	std::string m_ServerIp;
	int m_ServerPort;

	SLinkState& operator=(const SLinkState &rhs);
};


///
inline SLinkState::SLinkState(const SLinkState &rhs) 
{
	SLinkState::operator=(rhs);
}

/// 
inline SLinkState& SLinkState::operator=(const SLinkState &rhs)
{
	if (this != &rhs)
	{
		m_SvrType = rhs.m_SvrType;
		m_State = rhs.m_State;
		m_Type = rhs.m_Type;
		m_ServerIp = rhs.m_ServerIp;
		m_ServerPort = rhs.m_ServerPort;
	}
	return *this;
}
