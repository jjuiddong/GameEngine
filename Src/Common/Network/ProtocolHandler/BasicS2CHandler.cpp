
#include "stdafx.h"
#include "BasicS2CHandler.h"
#include "../Controller/P2PClient.h"
#include "../Service/Client.h"

using namespace network;


CBasicS2CHandler::CBasicS2CHandler( CClient &client ) :
	m_Client(client)
,	m_ClientState(CLIENT_END_MOVE)
{
	client.RegisterProtocol(&m_BasicProtocol);

	NETEVENT_CONNECT_TO(&client, this, EVT_CONNECT, CBasicS2CHandler, CBasicS2CHandler::OnConnectClient);
	NETEVENT_CONNECT_TO(&client, this, EVT_DISCONNECT, CBasicS2CHandler, CBasicS2CHandler::OnDisconnectClient);
}

CBasicS2CHandler::~CBasicS2CHandler()
{

}


/**
 @brief AckLogIn
 */
bool CBasicS2CHandler::AckLogIn(basic::AckLogIn_Packet &packet)
{ 
	if (error::ERR_SUCCESS == packet.errorCode)
	{
		if (CLIENT_CONNECT == m_ClientState)
		{
			m_ClientState = CLIENT_END_MOVE;
		}
		else
		{
			m_Client.SetName(packet.id);
			m_Client.SetCertifyKey(packet.c_key);
		}
	}
	return true; 
}


/**
 @brief Acknowlege packet of RequestP2PConnect 
 */
bool CBasicS2CHandler::AckP2PConnect(basic::AckP2PConnect_Packet &packet) 
{
	if (packet.errorCode != error::ERR_SUCCESS)
		return false; // todo: error process

	if (!m_Client.m_pP2p)
		return false; // error!!
	
	bool result = false;
	if (packet.state == P2P_HOST)
	{
		result = m_Client.m_pP2p->Bind( packet.port );
	}
	else if(packet.state == P2P_CLIENT)
	{
		result = m_Client.m_pP2p->Connect(packet.ip, packet.port);
	}

	m_BasicProtocol.ReqP2PConnectTryResult(SERVER_NETID, SEND_TARGET, result);
	return true;
}


/**
 @brief AckMoveToServer
 */
bool CBasicS2CHandler::AckMoveToServer(basic::AckMoveToServer_Packet &packet)
{
	if (error::ERR_SUCCESS != packet.errorCode)
		return false;

	m_Client.Stop();

	m_ClientState = CLIENT_BEGIN_MOVE;
	m_Client.SetIp(packet.ip);
	m_Client.SetPort(packet.port);
	return true;
}


/**
 @brief OnDisconnectClient
 */
void	CBasicS2CHandler::OnDisconnectClient(CNetEvent &event)
{
	if (CLIENT_BEGIN_MOVE == m_ClientState)
	{
		m_ClientState = CLIENT_CLOSE;
		StartClient(m_Client.GetIp(), m_Client.GetPort(), &m_Client);
	}	
}


/**
 @brief OnConnectClient
 */
void	CBasicS2CHandler::OnConnectClient(CNetEvent &event)
{
	if (CLIENT_CLOSE == m_ClientState)
	{
		m_ClientState = CLIENT_CONNECT;
		clog::Log( clog::LOG_F_N_O, clog::LOG_PACKET, 1, "MoveToServer ReqLogin id=%s, c_key=%d", 
			m_Client.GetName().c_str(), m_Client.GetCertifyKey() );
		m_BasicProtocol.ReqLogIn( SERVER_NETID, SEND_T, m_Client.GetName(), "", m_Client.GetCertifyKey());
	}
}
