/**
Name:   RemoteServer.h
Author:  jjuiddong
Date:    4/25/2013

	������ �ٸ� ������� ����� ��, ���� ������ ������ ǥ���Ѵ�.
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
