
#include "stdafx.h"
#include "Client.h"
#include "../ProtocolHandler/BasicS2CHandler.h"

using namespace network;


CClient::CClient(PROCESS_TYPE procType) : 
	CClientBasic(procType)
,	m_pBasicS2CProtocolHandler(NULL)
{

	m_pBasicS2CProtocolHandler = new CBasicS2CHandler(*this);
	AddProtocolListener(m_pBasicS2CProtocolHandler);
}

CClient::~CClient()
{
	RemoveProtocolListener(m_pBasicS2CProtocolHandler);
	SAFE_DELETE(m_pBasicS2CProtocolHandler);

}

