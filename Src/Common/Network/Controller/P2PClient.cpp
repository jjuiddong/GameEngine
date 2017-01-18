
#include "stdafx.h"
#include "P2PClient.h"
#include "CoreClient.h"
#include "Controller.h"

using namespace network;

CP2PClient::CP2PClient(PROCESS_TYPE procType) :
	CPlug(procType)
,	m_pP2pClient(NULL)
,	m_pP2pHost(NULL)
,	m_State(P2P_CLIENT)
{

}

CP2PClient::~CP2PClient()
{
	Clear();
}


/**
 @brief Create P2P Host Client 
 */
bool	CP2PClient::Bind( const int port )
{
	m_State = P2P_HOST;
	const bool result = CreateP2PHost(port);
	return result;
}


//------------------------------------------------------------------------
// P2P 통신을 시도한다.
// Client = P2P Client 상태가 되고, ip, port 로 접속을 시도한다.
//------------------------------------------------------------------------
bool	CP2PClient::Connect( const std::string &ip, const int port )
{
	m_State = P2P_CLIENT;
	const bool result = CreateP2PClient(ip,port);
	return result;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	CP2PClient::Stop()
{
	CController::Get()->StopServer(m_pP2pHost);
	CController::Get()->StopCoreClient(m_pP2pClient);
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	CP2PClient::Proc()
{
	switch (m_State)
	{
	case P2P_HOST: 
		if (m_pP2pHost)
			m_pP2pHost->Proc();
		break;

	case P2P_CLIENT:
		if (m_pP2pClient)
			m_pP2pClient->Proc();
		break;
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void	CP2PClient::Clear()
{
	CController::Get()->StopServer(m_pP2pHost);
	CController::Get()->StopCoreClient(m_pP2pClient);
	SAFE_DELETE(m_pP2pHost);
	SAFE_DELETE(m_pP2pClient);
}


/**
 @brief 
 */
bool	CP2PClient::Send(netid netId, const SEND_FLAG flag, CPacket &packet)
{
	return SendAll(packet);
}


//------------------------------------------------------------------------
// send to p2p host client
//------------------------------------------------------------------------
bool	CP2PClient::SendAll(CPacket &packet)
{
	switch (m_State)
	{
	case P2P_CLIENT: 
		if (!m_pP2pClient)
			return false;
		m_pP2pClient->Send(P2P_NETID, SEND_TARGET, packet);
		break;

	case P2P_HOST:
		m_pP2pHost->SendAll(packet);
		break;
	}
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	CP2PClient::CreateP2PHost( const int port )
{
	if (m_pP2pHost)
	{
		Close();
	}
	else
	{
		m_pP2pHost = new CServerBasic(GetProcessType());
		AddChild(m_pP2pHost);
		m_pP2pHost->EventConnect( this, EVT_LISTEN, NetEventHandler(CP2PClient::OnListen) );
		m_pP2pHost->EventConnect( this, EVT_DISCONNECT, NetEventHandler(CP2PClient::OnDisconnect) );
		m_pP2pHost->EventConnect( this, EVT_CLIENT_JOIN, NetEventHandler(CP2PClient::OnClientJoin) );
		m_pP2pHost->EventConnect( this, EVT_CLIENT_LEAVE, NetEventHandler(CP2PClient::OnClientLeave) );

		BOOST_FOREACH(auto &pPrt, GetProtocolListeners())
		{
			m_pP2pHost->AddProtocolListener( pPrt );
		}
	}

	return CController::Get()->StartServer(port, m_pP2pHost);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	CP2PClient::CreateP2PClient( const std::string &ip, const int port )
{
	if (m_pP2pClient)
	{
		Close();
	}
	else
	{
		m_pP2pClient = new CCoreClient(GetProcessType());
		AddChild(m_pP2pClient);
		m_pP2pClient->EventConnect( this, EVT_CONNECT, NetEventHandler(CP2PClient::OnConnect) );
		m_pP2pClient->EventConnect( this, EVT_DISCONNECT, NetEventHandler(CP2PClient::OnDisconnect) );

		BOOST_FOREACH(auto &pPrt, GetProtocolListeners())
		{
			m_pP2pClient->AddProtocolListener( pPrt );
		}
	}

	return CController::Get()->StartCoreClient(ip, port, m_pP2pClient);
}


/**
@brief  close socket and remove link CNetController
*/
void	CP2PClient::Disconnect()
{
	if (m_pP2pClient)
		m_pP2pClient->Disconnect();
	if (m_pP2pHost)
		m_pP2pHost->Disconnect();
}


/**
@brief  close socket
*/
void	CP2PClient::Close()
{
	if (m_pP2pClient)
		m_pP2pClient->Close();
	if (m_pP2pHost)
		m_pP2pHost->Close();
}


//------------------------------------------------------------------------
// p2pHost/Client 두 객체에게 Protocol Listener을 등록한다.
//------------------------------------------------------------------------
//bool	CP2PClient::AddProtocolListener(ProtocolListenerPtr pListener)
//{
//	if (!CPlug::AddProtocolListener(pListener))
//		return false;
//	if (m_pP2pClient)
//		m_pP2pClient->AddProtocolListener(pListener);
//	if (m_pP2pHost)
//		m_pP2pHost->AddProtocolListener(pListener);
//	return true;
//}


//------------------------------------------------------------------------
// p2pHost/Client 두 객체에게 적용한다.
//------------------------------------------------------------------------
//bool	CP2PClient::RemoveProtocolListener(ProtocolListenerPtr pListener)
//{
//	if (!CPlug::RemoveProtocolListener(pListener))
//		return false;
//	if (m_pP2pClient)
//		m_pP2pClient->RemoveProtocolListener(pListener);
//	if (m_pP2pHost)
//		m_pP2pHost->RemoveProtocolListener(pListener);
//	return true;
//}


/**
 @brief P2P Host Event Handler , P2P host Listen
 */
void	CP2PClient::OnListen(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	event.Skip();
	SearchEventTable( CNetEvent(EVT_CONNECT, this));
}


/**
 @brief P2P Host Event Handler , P2P connect
 */
void	CP2PClient::OnConnect(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	event.Skip();
	SearchEventTable( CNetEvent(EVT_CONNECT, this));
}


/**
 @brief P2P Host Event Handler , P2P disconnect
 */
void	CP2PClient::OnDisconnect(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	event.Skip();
	SearchEventTable( CNetEvent(EVT_DISCONNECT, this));
}


/**
 @brief P2P Host Event Handler , P2P Client join
 */
void	CP2PClient::OnClientJoin(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	event.Skip();

	if (m_State == P2P_HOST)
		SendAll( P2PMemberJoin(event.GetNetId()) );

	SearchEventTable( CNetEvent(EVT_MEMBER_JOIN, this, event.GetNetId()));
}


/**
 @brief P2P Host Event Handler , P2P Client leave
 */
void	CP2PClient::OnClientLeave(CNetEvent &event)
{
	if (this == event.GetEventObject())
		return;

	event.Skip();

	if (m_State == P2P_HOST)
		SendAll( P2PMemberLeave(event.GetNetId()) );

	SearchEventTable( CNetEvent(EVT_MEMBER_LEAVE, this, event.GetNetId()));
}
