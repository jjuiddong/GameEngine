/**
Name:   PlayerAccess.h
Author:  jjuiddong
Date:    3/17/2013

	interface class
	get the Session in Server Object or Remote Client in Client Object
*/
#pragma once

namespace network
{

	/// ISessionAccess
	class ISessionAccess
	{
	public:
		virtual ~ISessionAccess() {}
		virtual SessionPtr GetSession(netid id) = 0;
	};


	/// IPlayerAcess
	class IPlayerAccess
	{
	public:
		virtual ~IPlayerAccess() {}
		virtual PlayerPtr GetPlayer(netid id) = 0;
	};

}
