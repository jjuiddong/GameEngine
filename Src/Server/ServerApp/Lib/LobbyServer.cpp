
#include "stdafx.h"
#include "LobbyServer.h"
#include "../DataStructure/LobbyPlayer.h"


using namespace network;

CLobbyServer::CLobbyServer() :
	m_pBasicPrtHandler(NULL)
{

}

CLobbyServer::~CLobbyServer() 
{
//	RemoveProtocolListener(m_pBasicPrtHandler);
	SAFE_DELETE(m_pBasicPrtHandler);

}


/**
 @brief OnConnectNetGroupController
 */
void	CLobbyServer::OnConnectMultiPlug()
{
	CServer::OnConnectMultiPlug();

	MultiPlugPtr pLoginMultiPlug = multinetwork::CMultiNetwork::Get()->GetMultiPlug("loginsvr");
	if (!pLoginMultiPlug)
	{
		clog::Error( clog::ERROR_CRITICAL, "CLobbyServer Init Error!! not found lobbysvr multiplug" );
		return;
	}
	MultiPlugPtr pCertifyMultiPlug = multinetwork::CMultiNetwork::Get()->GetMultiPlug("certifysvr");
	if (!pCertifyMultiPlug)
	{
		clog::Error( clog::ERROR_CRITICAL, "CLobbyServer Init Error!! not found certify multiplug" );
		return;
	}
	MultiPlugPtr pGameMultiPlug = multinetwork::CMultiNetwork::Get()->GetMultiPlug("gamesvr");
	if (!pGameMultiPlug)
	{
		clog::Error( clog::ERROR_CRITICAL, "CLobbyServer Init Error!! not found gamesvr multiplug" );
		return;
	}

	AddChild( pLoginMultiPlug );
	AddChild( pCertifyMultiPlug );
	AddChild( pGameMultiPlug );

	GetServer()->SetOption( true );
	GetServer()->SetPlayerFactory( new CLobbyPlayerFactory() );
	
	m_pBasicPrtHandler = new CBasicC2SHandler_LobbySvr(*pCertifyMultiPlug, *GetServer());

	pLoginMultiPlug->RegisterProtocol( &m_SvrNetworkProtocol );
	//pCertifyMultiPlug->RegisterProtocol(&m_CertifyProtocol);

	RegisterProtocol(&m_LoginProtocol);
	RegisterProtocol(&m_BasicProtocol);
	AddProtocolListener( this );
	AddProtocolListener(m_pBasicPrtHandler);

	NETEVENT_CONNECT( EVT_CLIENT_JOIN, CLobbyServer, CLobbyServer::OnClientJoin );
	NETEVENT_CONNECT( EVT_CLIENT_LEAVE, CLobbyServer, CLobbyServer::OnClientLeave );
	EVENT_CONNECT_TO( GetServer(), this, EVT_TIMER, CLobbyServer, CLobbyServer::OnTimer );

	GetServer()->AddTimer(ID_TIMER_REFRESH, REFRESH_TIMER_INTERVAL);
}



////------------------------------------------------------------------------
//// �� �߰�
////------------------------------------------------------------------------
//bool CLobbyServer::AddRoom(CRoom *pRoom)
//{
//	RoomItor it = m_Rooms.find(pRoom->GetId());
//	if (m_Rooms.end() != it)
//		return false; // �̹� �����Ѵٸ� ����
//	m_Rooms.insert( RoomMap::value_type(pRoom->GetId(), pRoom) );
//	return true;
//}
//
//
////------------------------------------------------------------------------
//// ������
//// �޸𸮱��� ���ŵȴ�.
////------------------------------------------------------------------------
//bool CLobbyServer::RemoveRoom(CRoom *pRoom)
//{
//	RoomItor it = m_Rooms.find(pRoom->GetId());
//	if (m_Rooms.end() == it)
//		return false; // ���ٸ� ����
//	delete it->second;
//	m_Rooms.erase(it);
//	return true;
//}
//
//
////------------------------------------------------------------------------
//// ������
//// �޸𸮱��� ���ŵȴ�.
////------------------------------------------------------------------------
//bool CLobbyServer::RemoveRoom(int roomId)
//{
//	RoomItor it = m_Rooms.find(roomId);
//	if (m_Rooms.end() == it)
//		return false; // ���ٸ� ����
//	delete it->second;
//	m_Rooms.erase(it);
//	return true;
//}
//
//
////------------------------------------------------------------------------
//// ������ ���
////------------------------------------------------------------------------
//RoomPtr CLobbyServer::GetRoom(int roomId)
//{
//	RoomItor it = m_Rooms.find(roomId);
//	if (m_Rooms.end() == it)
//		return NULL; // ���ٸ� ����
//	return it->second;
//}


////------------------------------------------------------------------------
//// �������� userId Ŭ���̾�Ʈ���� ������.
////------------------------------------------------------------------------
//void CLobbyServer::SendRooms(netid userId)
//{
//	BOOST_FOREACH(RoomMap::value_type &vk, m_Rooms)
//	{
//
//	}
//}


//------------------------------------------------------------------------
// ���������� userId Ŭ���̾�Ʈ���� ������.
//------------------------------------------------------------------------
void CLobbyServer::SendUsers(netid userId)
{

}


//------------------------------------------------------------------------
// Ŭ���̾�Ʈ�� ������ �پ��� �� ȣ��ȴ�.
//------------------------------------------------------------------------
void CLobbyServer::OnClientJoin(CNetEvent &event)
{	
//	CLobbyUser *pUser = new CLobbyUser();
//	pUser->SetNetId(event.GetNetId());
//	if (!AddUser( pUser ))
//	{
//		clog::Error( clog::ERROR_PROBLEM, "AddUser() Fail!! id = %d\n", event.GetNetId());
//		delete pUser;
//	}
}


//------------------------------------------------------------------------
// Ŭ���̾�Ʈ�� ������ ���� ������ ������ �� ȣ��ȴ�.
//------------------------------------------------------------------------
void CLobbyServer::OnClientLeave(CNetEvent &event)
{
	RET(!GetServer());

	if (!GetServer()->RemovePlayer(event.GetNetId()))
	{
		clog::Error(clog::ERROR_PROBLEM,  "RemoveUser() Fail!! id = %d\n", event.GetNetId());
	}
}


//------------------------------------------------------------------------
// ��Ʈ������ ��ȯ, �ַ� ����뿡 ���õ� ������ ��Ʈ������ ������.
//------------------------------------------------------------------------
std::string CLobbyServer::ToString()
{
	common::AutoCSLock cs(GetServer()->GetCS());

	std::stringstream ss;
	ss << "Session : " << GetServer()->GetSessions().size() << std::endl;
	BOOST_FOREACH(auto &client, GetServer()->GetSessions())
	{
		ss << "netid: " << client->GetNetId() << ", sock: " << client->GetSocket() << std::endl;
	}
	return ss.str();
}


/**
 @brief 
 */
void	CLobbyServer::OnTimer( CEvent &event )
{
	if (ID_TIMER_REFRESH == event.GetParam())
	{
		// �ֱ������� ���� ������ Login�������� ������.
		if (GetServer() && GetServer()->IsServerOn())
		{
			MultiPlugPtr pLoginSvrController = multinetwork::CMultiNetwork::Get()->GetMultiPlug("loginsvr");
			if (pLoginSvrController)
			{
				pLoginSvrController->RegisterProtocol( &m_SvrNetworkProtocol );
				m_SvrNetworkProtocol.SendServerInfo( ALL_NETID, network::SEND_T, "lobbysvr", 
					GetServer()->GetIp(), GetServer()->GetPort(), GetServer()->GetSessions().size() );
			}
		}
	}
}


/**
 @brief ReqMovePlayer
			From Login Server
			From Game Server
 */
bool CLobbyServer::ReqMovePlayer(server_network::ReqMovePlayer_Packet &packet)
{
	CSession *pSession = CheckClientId(GetServer(), packet.id, 0, NULL, NULL);
	if (pSession) // Already exist
	{ /// !!Error
		clog::Error( clog::ERROR_PROBLEM, 0, "ReqMovePlayer Player already exist netid: %d, id=%s", 
			pSession->GetNetId(), pSession->GetName().c_str() );
		m_SvrNetworkProtocol.AckMovePlayer(packet.senderId, SEND_T, error::ERR_MOVEUSER_ALREADY_EXIST,
			packet.id, packet.groupId, packet.ip, packet.port);
		return false;
	}

	// Add User
	CPlayer *pPlayer = GetServer()->GetPlayerFactory()->New();
	pPlayer->SetName(packet.id);
	pPlayer->SetCertifyKey(packet.c_key);
	GetServer()->AddPlayer( pPlayer );
	
	m_SvrNetworkProtocol.AckMovePlayer(packet.senderId, SEND_T, error::ERR_SUCCESS, 
		packet.id, packet.groupId, packet.ip, packet.port);	
	return true;
}
