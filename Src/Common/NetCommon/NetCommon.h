/**
Name:    NetCommon.h
Author:  jjuiddong
Date:    2013-03-26

*/
#pragma once

#include <Windows.h>
#include <winsock.h>


typedef int netid;
typedef int certify_key;
#define INVALID_NETID		(-1)		// �߸��� NetId�� ���Ѵ�. ���������� ����
#define SERVER_NETID		(0)		// ����� Server�� NetId�� ���� (Ŭ���̾�Ʈ �� ��)
#define ALL_NETID				(0)		// ����� Connection ��θ� ���� (����/Ŭ���̾�Ʈ)
#define P2P_NETID				(1)		// ����� P2P ��Ʈ��ũ Host NetId�� ���� (Ŭ���̾�Ʈ �� ��)
#define ROOT_GROUP_NETID (-1)	// ������ �ֻ��� �׷��� ���Ѵ�.

// Option
static const bool DISPLAY_PACKET_LOG = true;




namespace network
{
	using namespace common;

	class CPlug;
	class CGroup;
	class CRemoteClient;
	class CRemoteServer;
	class CSession;
	class CPacket;
	class IProtocol;
	class IProtocolListener;
	class CCharacter;
	class CPlayer;


	enum SERVICE_TYPE
	{
		CLIENT,
		SERVER,
	};

	enum CONNECTOR_TYPE
	{
		CON_CLIENT,
		CON_CORECLIENT,
		CON_SERVER,
	};

	enum PROCESS_TYPE
	{
		USER_LOOP,									// ������ �����忡�� ó��
		SERVICE_SEPERATE_THREAD,		// ���� �����忡�� ó�� (����ó���� ���� ���� �и��ص� �����带 ���Ѵ�)
		SERVICE_EXCLUSIVE_THREAD,		// ������ �����带 �����ؼ� ó��
		SERVICE_CHILD_THREAD,				// �θ� �����忡�� ó���Ѵ�. ���� �θ� USER_LOOP ���, �ڽĵ� USER_LOOP�� �����Ѵ�.
	};

	enum SESSION_STATE
	{
		SESSIONSTATE_DISCONNECT,
		SESSIONSTATE_LOGIN_WAIT,
		SESSIONSTATE_LOGIN,					// ������ �� ����
		SESSIONSTATE_LOGOUT_WAIT,		// ���� ��� ��Ͽ� �ִ� ����
	};

	enum PLAYER_STATE
	{
		USERSTATE_LISTEN,
		USERSTATE_WAIT_ACK,
	};
	
	enum P2P_STATE
	{
		P2P_HOST,
		P2P_CLIENT,
	};

	/// Send Packet Flag, composite flag
	enum SEND_FLAG
	{
		SEND_TARGET,
		SEND_VIEWER,
		SEND_TARGET_VIEWER,
		SEND_T = SEND_TARGET,
		SEND_V = SEND_VIEWER,
		SEND_T_V = SEND_TARGET_VIEWER,
	};




	struct SHostInfo
	{
		std::string ip;
		int portnum;

		SHostInfo() {}
		SHostInfo(std::string ip_, int port) : ip(ip_), portnum(port) {}
	};


	struct SSubServerInfo : SHostInfo
	{
		netid serverId;
		int userCnt;

		bool operator<( const SSubServerInfo &rhs) { return userCnt < rhs.userCnt; }
	};


}


typedef std::list<SOCKET> SocketList;
typedef SocketList::iterator SockItor;

typedef std::list<netid> NetIdList;
typedef NetIdList::iterator NetIdItor;
typedef std::vector<netid> NetIdes;

typedef std::list<network::CPacket> PacketList;
typedef PacketList::iterator PacketItor;
typedef PacketList::const_iterator PacketCItor;


typedef common::StableVectorMap<netid,network::CPlayer*> Players_;
//typedef Users_::iterator UserItor;
typedef common::ReferencePtr<network::CPlayer> PlayerPtr;
typedef common::ReferencePtr<network::CCharacter> CharacterPtr;


typedef common::ReferencePtr<network::CPlug> PlugPtr;
typedef common::StableVectorMap<netid, PlugPtr> Plugs;
typedef Plugs::VectorType PlugsV;
typedef common::StableVectorMap<netid, network::CPlug*> Plugs_;
typedef Plugs_::VectorType Plugs_V;

typedef common::VectorMap<netid,network::CRemoteClient*> RemoteClients;
typedef RemoteClients::iterator RemoteClientItor;
typedef common::ReferencePtr<network::CRemoteClient> RemoteClientPtr;
typedef common::ReferencePtr<network::CRemoteServer> RemoteServerPtr;

typedef common::ReferencePtr<network::CSession> SessionPtr;
typedef common::StableVectorMap<netid,network::CSession*> Sessions_;
typedef Sessions_::VectorType	Sessions_V;
typedef Sessions_::MapType::iterator SessionItor;


typedef std::list<network::CGroup*> GroupList;
typedef GroupList::iterator GroupItor;
typedef common::ReferencePtr<network::CGroup> GroupPtr;
typedef common::VectorMap<netid, network::CGroup*> Groups;

typedef common::ReferencePtr<network::IProtocol> ProtocolPtr;
typedef common::ReferencePtr<network::IProtocolListener> ProtocolListenerPtr;
typedef std::list<ProtocolListenerPtr> ProtocolListenerList;
typedef ProtocolListenerList::iterator ProtocolListenerItor;
typedef std::map<int,ProtocolListenerList> ProtocolListenerMap;
typedef ProtocolListenerMap::iterator ProtocolListenerMapItor;
typedef std::map<int,ProtocolPtr> ProtocolMap;
typedef ProtocolMap::iterator ProtocolItor;



#include "DataStructure/Packet.h"
#include "DataStructure/PacketQueue.h"
#include "DataStructure/RemoteClient.h"
#include "DataStructure/RemoteServer.h"
#include "DataStructure/Group.h"
#include "DataStructure/Player.h"

#include "Marshalling/Marshalling.h"
#include "Marshalling/MarshallingGroup.h"
#include "Marshalling/MarshallingEtc.h"

#include "Controller/Plug.h"
#include "Controller/PlugLinker.h"
#include "Controller/Session.h"

#include "Interface/Protocol.h"
#include "Interface/ProtocolDispatcher.h"
#include "Interface/ProtocolListener.h"
#include "Interface/Factory.h"

#include "PrtCompiler/ProtocolDefine.h"
#include "PrtCompiler/ProtocolParser.h"
#include "PrtCompiler/ProtocolMacro.h"

#include "Event/NetEvent.h"

#include "Utility/PlayerAccess.h"
#include "Utility/Protocols.h"

#include "Error/ErrorCode.h"
