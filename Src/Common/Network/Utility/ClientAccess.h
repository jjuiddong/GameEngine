/**
Name:   ClientUserAccess.h
Author:  jjuiddong
Date:    3/17/2013

adapter class for access the user object in CClient class object
*/
namespace network
{
	class CClientSessionAccess : public ISessionAccess
	{
	public:
		CClientSessionAccess( ClientBasicPtr svr );
		virtual ~CClientSessionAccess();
		virtual SessionPtr GetSession(netid id) override;
	protected:
		ClientBasicPtr m_pSvr;
	};

	// not implements


	class CClientPlayerAccess : public IPlayerAccess
	{
	public:
		CClientPlayerAccess( ClientBasicPtr svr );
		virtual ~CClientPlayerAccess();
		virtual SessionPtr GetPlayer(netid id) override;
	protected:
		ClientBasicPtr m_pSvr;
	};

}
