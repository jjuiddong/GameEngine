/**
Name:   RemoteServer.h
Author:  jjuiddong
Date:    4/25/2013

	서버가 다른 서버들과 통신할 때, 원격 서버의 정보를 표현한다.
*/
#pragma once

#include "../Controller/Session.h"

namespace network
{

	class CRemoteServer : public CSession
	{
	public:
		CRemoteServer();
		virtual ~CRemoteServer();

		void		SetUserCount(int cnt);
		int		GetUserCount() const;

	protected:

	private:
		int m_UserCount;
	};


	inline void		CRemoteServer::SetUserCount(int cnt) { m_UserCount = cnt; }
	inline int		CRemoteServer::GetUserCount() const { return m_UserCount; }

}
