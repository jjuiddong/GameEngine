
#include "stdafx.h"
#include "CertifyServer.h"
#include "GlobalRemotePlayer.h"

#include "NetProtocol/src/certify_Protocol.cpp"
#include "NetProtocol/src/certify_ProtocolListener.cpp"

using namespace network;

CCertifyServer::CCertifyServer()
{

}

CCertifyServer::~CCertifyServer()
{
	Clear();
}


/**
 @brief OnConnectNetGroupController
 */
void	CCertifyServer::OnConnectMultiPlug()
{
	CServer::OnConnectMultiPlug();

	AddProtocolListener(this);
	RegisterProtocol( &m_Protocol );

	NETEVENT_CONNECT( EVT_CLIENT_LEAVE, CCertifyServer, CCertifyServer::OnClientLeave );
}


/**
 @brief OnClientLeave
 */
void	CCertifyServer::OnClientLeave(CNetEvent &event)
{
	CSession *pSession = CheckClientNetId(GetServer(), event.GetNetId(), NULL, NULL);
	RET(!pSession);

	PlayerLogoutConnectingServer( pSession->GetName() );
}


/**
 @brief LogOutCorrespondServer
 */
void	CCertifyServer::PlayerLogoutConnectingServer( const std::string &svrType )
{
	BOOST_FOREACH(auto player, m_Sessions.m_Seq)
	{
		if (player->GetLocateSvrType() == svrType)
			m_Sessions.remove( player->GetName() );
	}
	m_Sessions.apply_removes();
}


/**
 @brief 
 */
void	CCertifyServer::Clear()
{
	BOOST_FOREACH(auto pSession, m_Sessions.m_Seq)
	{
		delete pSession;
	}
	m_Sessions.clear();
}


/**
 @brief Add New Session
 */
bool	CCertifyServer::AddPlayer( const std::string &id, const std::string &svrType, const netid svrId, 
	const certify_key key  )
{
	auto it = m_Sessions.find( id );
	if (m_Sessions.end() != it)
		return false; /// Error!! Already Exist

	if (id.empty())
		return false;

	CGlobalRemotePlayer *pNewClient = new CGlobalRemotePlayer();
	pNewClient->SetState( SESSIONSTATE_LOGIN );
	pNewClient->SetNetId( 0 );
	pNewClient->SetName( id );
	pNewClient->SetLocateSvrId( svrId );
	pNewClient->SetLocateSvrType(svrType);
	return m_Sessions.insert( SessionsId::value_type(id, pNewClient) );
}


/**
 @brief RemoveSession
 */
bool	CCertifyServer::RemovePlayer( const std::string &id )
{
	auto it = m_Sessions.find( id );
	if (m_Sessions.end() == it)
		return false; /// Error!! not found session

	delete it->second;
	m_Sessions.remove(id);
	m_Sessions.apply_removes();
	return true;
}


/**
 @brief 
 */
CGlobalRemotePlayer* CCertifyServer::GetPlayer( const std::string &id )
{
	auto it = m_Sessions.find( id );
	if (m_Sessions.end() == it)
		return NULL; /// Error!! not found session
	return it->second;
}


/**
 @brief ReqUserLogin
 */
bool CCertifyServer::ReqUserLogin(certify::ReqUserLogin_Packet &packet)
{
	certify_key key = rand();

	if (!AddPlayer(packet.id, packet.svrType, packet.senderId, key))
	{ /// Error!! Already Exist 
		m_Protocol.AckUserLogin( packet.senderId, SEND_T, error::ERR_ALREADY_EXIST_USER, packet.id, 0 );
		return false;
	}

	m_Protocol.AckUserLogin(packet.senderId, SEND_T, error::ERR_SUCCESS, packet.id, key );
	return true;
}


/**
 @brief ReqUserLogout
 */
bool CCertifyServer::ReqUserLogout(certify::ReqUserLogout_Packet &packet)
{
	// Remove Session
	if (!RemovePlayer(packet.id))
	{ /// Error!! Not Exist 
		m_Protocol.AckUserLogout( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.id );
		return false;
	}

	m_Protocol.AckUserLogout( packet.senderId, SEND_T, error::ERR_SUCCESS, packet.id );
	return true;
}


/**
 @brief ReqUserMoveServer
 */
bool CCertifyServer::ReqUserMoveServer(certify::ReqUserMoveServer_Packet &packet)
{ 
	CGlobalRemotePlayer *pClient = GetPlayer(packet.id);
	if (!pClient)
	{ /// Error!!!
		clog::Error( clog::ERROR_PROBLEM, "ReqUserMoveServer Error!! not found client id=%s",
			packet.id.c_str() );
		m_Protocol.AckUserMoveServer( packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.id, packet.svrType);
	}

	pClient->SetLocateSvrType(packet.svrType);
	m_Protocol.AckUserMoveServer( packet.senderId, SEND_T, error::ERR_SUCCESS, packet.id, packet.svrType);
	return true;
}


/**
 @brief SendServerInfo
 */
bool CCertifyServer::SendServerInfo(server_network::SendServerInfo_Packet &packet)
{
	CSession *pSession = CheckClientNetId(GetServer(), packet.senderId, NULL, packet.pdispatcher);
	RETV(!pSession, false);

	pSession->SetName(packet.svrType);
	return true;
}
