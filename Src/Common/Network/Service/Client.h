/**
Name:   Client.h
Author:  jjuiddong
Date:    3/17/2013

*/
#pragma once

#include "../Controller/ClientBasic.h"

namespace network
{
	class CBasicS2CHandler;

	class CClient : public CClientBasic
	{
	public:
		CClient(PROCESS_TYPE procType);
		virtual ~CClient();

	protected:
		CBasicS2CHandler *m_pBasicS2CProtocolHandler;
	};

};
