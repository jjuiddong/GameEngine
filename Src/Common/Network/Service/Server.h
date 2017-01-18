/**
Name:   Server.h
Author:  jjuiddong
Date:    2013-03-12

Group Management
*/
#pragma once

#include "../Controller/ServerBasic.h"

namespace basic { class s2c_Protocol; }

namespace network
{
	class CBasicC2SHandler;
	class CServer : public multinetwork::CMultiPlugDelegation
	{
		friend class CBasicC2SHandler;

	public:
		CServer();
		virtual ~CServer();

	protected:
		// Event Handler
		virtual void	OnConnectMultiPlug() override; // must call derived class

		// Network Event Handler
		void OnClientJoin(CNetEvent &event);
		void OnClientLeave(CNetEvent &event);

	private:
		basic::s2c_Protocol		*m_pBasicProtocol;
		CBasicC2SHandler *m_pBasicPrtHandler;
	};
}
