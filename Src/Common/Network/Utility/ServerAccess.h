/**
Name:   ServerUserAccess.h
Author:  jjuiddong
Date:    3/17/2013

adapter class for server user map access 
*/
#pragma once

#include "../Controller/ServerBasic.h"

namespace network
{


	class CServerSessionAccess : public ISessionAccess
	{
	public:
		CServerSessionAccess( ServerBasicPtr svr );
		virtual SessionPtr GetSession(netid id) override;
	protected:
		ServerBasicPtr m_pSvr;
	};


	inline CServerSessionAccess::CServerSessionAccess( ServerBasicPtr svr ) : 
		m_pSvr(svr)  { }

		/**
	 @brief return the remote client in server object
	 */
	inline SessionPtr CServerSessionAccess::GetSession(netid id)
	{
		RETV(!m_pSvr, NULL);
		return m_pSvr->GetSession(id);
	}




	class CServerPlayerAccess : public IPlayerAccess
	{
	public:
		CServerPlayerAccess( ServerBasicPtr svr );
		virtual PlayerPtr GetPlayer(netid id) override;
	protected:
		ServerBasicPtr m_pSvr;
	};


	inline CServerPlayerAccess::CServerPlayerAccess( ServerBasicPtr svr ) : 
		m_pSvr(svr)  { }

	/**
	 @brief return the remote client in server object
	 */
	inline PlayerPtr CServerPlayerAccess::GetPlayer(netid id)
	{
		RETV(!m_pSvr, NULL);
		return m_pSvr->GetPlayer(id);
	}


}
