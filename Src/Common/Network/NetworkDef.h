
#pragma once


#include <Windows.h>
#include <winsock.h>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include "Common/Common.h"
#include "NetCommon/NetCommon.h"

#include "wxMemMonitorLib/wxMemMonitor.h"


namespace network
{
	class CServerBasic;
	class CClientBasic;
	class CClient;
	class CCoreClient;
	class CP2PClient;
	class CRoom;

}

typedef common::ReferencePtr<network::CServerBasic> ServerBasicPtr;
typedef common::ReferencePtr<network::CClientBasic> ClientBasicPtr;
typedef common::ReferencePtr<network::CClient> ClientPtr;
typedef common::ReferencePtr<network::CP2PClient> P2PClientPtr;
typedef common::ReferencePtr<network::CCoreClient> CoreClientPtr;


typedef common::StableVectorMap<netid, ServerBasicPtr> Servers;
typedef common::StableVectorMap<netid, ClientBasicPtr> Clients;
typedef common::StableVectorMap<netid, network::CClientBasic*> Clients_;
typedef Clients_::VectorType	Clients_V;
typedef common::StableVectorMap<netid, CoreClientPtr> CoreClients;
typedef common::StableVectorMap<netid, network::CCoreClient*> CoreClients_;
typedef CoreClients_::VectorType	CoreClients_V;

//typedef common::VectorMap<netid, CCoreClient*> CoreClientsPtr;


#include "Algorithm/BindCore.h"



typedef std::map<int,network::CRoom*> RoomMap;
typedef RoomMap::iterator RoomItor;
typedef common::ReferencePtr<network::CRoom> RoomPtr;


#include "Utility/Utility.h"

#include "DataStructure/Room.h"
#include "Service/AllProtocol.h"
#include "Service/AllProtocolListener.h"
#include "Service/ServerApp.h"


typedef std::vector<network::CGroup> GroupVector;



namespace network { namespace multinetwork {

	class CMultiPlug;
	class CMultiPlugDelegation;
}}

typedef common::ReferencePtr<network::multinetwork::CMultiPlug> MultiPlugPtr;
typedef common::ReferencePtr<network::multinetwork::CMultiPlugDelegation> MultiPlugDelegationPtr;


#include "Controller/CoreClient.h"

#include "MultiNetwork/MultiNetwork.h"
#include "MultiNetwork/MultiPlug.h"
#include "MultiNetwork/MultiPlugDelegation.h"

#include "ProtocolHandler\AllProtocolDisplayer.h"




#include "ErrReport/ErrorCheck.h"


// fastmemloader로 정보를 읽는다.
//#pragma pack(1)
//typedef struct _SServerInfo
//{
//	char	name[32];
//	char	s2sip[32];
//	int		s2sport;
//	int		s2cport;
//} SServerInfo;
//
//typedef struct _SServerLaunchInfo
//{
//	int lobbyServerCnt;
//	SServerInfo *lobbySvrs;
//	int gameServerCnt;
//	SServerInfo *gameSvrs;
//
//} SServerLaunchInfo;
//#pragma pack()

