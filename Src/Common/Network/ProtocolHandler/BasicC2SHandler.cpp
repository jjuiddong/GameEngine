
#include "stdafx.h"
#include "BasicC2SHandler.h"
#include "../Utility/ServerAccess.h"
#include "../Algorithm/GroupTraverse.h"
#include "Network/ErrReport/ErrorCheck.h"

#include "NetProtocol/Src/basic_ProtocolListener.cpp"
#include "NetProtocol/Src/basic_Protocol.cpp"

using namespace network;
using namespace network::error;

CBasicC2SHandler::CBasicC2SHandler( CServerBasic &svr ) :
	m_Server(svr)
{
	svr.RegisterProtocol(&m_BasicProtocol);
}

CBasicC2SHandler::~CBasicC2SHandler()
{
}


/**
 @brief ReqLogIn
 */
bool CBasicC2SHandler::ReqLogIn(basic::ReqLogIn_Packet &packet)
{
	PlayerPtr pPlayer = CheckPlayerWaitAck(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher);
	RETV(!pPlayer, false);

	CSession *pClient = m_Server.GetSession(packet.id);
	if (pClient)
	{
		if (pClient->GetState() == SESSIONSTATE_LOGIN)
		{
			clog::Error( clog::ERROR_PROBLEM, "ReqLogin Error!! client already exist senderId=%d, id=%s",
				packet.senderId, packet.id.c_str());
			m_BasicProtocol.AckLogIn(packet.senderId, SEND_T, error::ERR_ALREADY_EXIST_USER, packet.id, 0);
			return false;
		}
	}

	pClient = m_Server.GetSession(packet.senderId);
	if (!pClient)
	{
		clog::Error( clog::ERROR_PROBLEM, "ReqLogin Error!! session  not found senderId=%d, id=%s",
			packet.senderId, packet.id.c_str());
		m_BasicProtocol.AckLogIn(packet.senderId, SEND_T, error::ERR_NOT_FOUND_USER, packet.id, 0);
		return false;
	}

	if (pClient->GetState() != SESSIONSTATE_LOGIN_WAIT)
	{
		clog::Error( clog::ERROR_PROBLEM, "ReqLogin Error!! client state error state=%d",
			pClient->GetState() );
		m_BasicProtocol.AckLogIn(packet.senderId, SEND_T, error::ERR_INVALID_USER, packet.id, 0);
		return false;
	}

	pClient->SetName(packet.id);
	return true;
}


/**
 @brief ReqLogOut
 */
bool CBasicC2SHandler::ReqLogOut(basic::ReqLogOut_Packet &packet)
{
	if (!CheckRecvablePlayer(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher))
		return false;

	return true;
}


//------------------------------------------------------------------------
// groupid : -1 이라면 root 그룹의 자식을 보낸다.
//------------------------------------------------------------------------
bool CBasicC2SHandler::ReqGroupList(basic::ReqGroupList_Packet &packet)
{
	if (!CheckRecvablePlayer(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher))
		return false;

	GroupPtr pGroup = NULL;
	if (ROOT_GROUP_NETID == packet.groupid)
	{
		pGroup = &m_Server.GetRootGroup();
	}
	else
	{
		pGroup = m_Server.GetRootGroup().GetChildandThis(packet.groupid);
	}

	GroupVector gv;
	if (pGroup)
	{
		const Groups::VectorType &children = pGroup->GetChildren();
		gv.reserve(children.size());
		for (u_int i=0; i < children.size(); ++i)
			gv.push_back( *children[i] );
	}
	m_BasicProtocol.AckGroupList(packet.senderId, SEND_TARGET, 
		(pGroup)? ERR_SUCCESS : ERR_GROUPLIST_NOT_FOUND_GROUP, gv);
	return true;
}


//------------------------------------------------------------------------
// Request Joint the Group of groupid
//------------------------------------------------------------------------
bool CBasicC2SHandler::ReqGroupJoin(basic::ReqGroupJoin_Packet &packet)
{
	if (!CheckRecvablePlayer(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher))
		return false;

	GroupPtr pTo = (packet.groupid == INVALID_NETID)? &m_Server.GetRootGroup() : m_Server.GetRootGroup().GetChildandThis(packet.groupid);
	GroupPtr pFrom = m_Server.GetRootGroup().GetChildFromPlayer( packet.senderId );
	if (pTo && pFrom)
	{
		if (pTo->GetNetId() == pFrom->GetNetId())
		{// Error!!
			m_BasicProtocol.AckGroupJoin( packet.senderId, SEND_TARGET, ERR_GROUPJOIN_ALREADY_SAME_GROUP, 
				packet.senderId, packet.groupid );
			return false;
		}
		if (!pTo->IsTerminal())
		{// Error!!
			m_BasicProtocol.AckGroupJoin( packet.senderId, SEND_TARGET, ERR_GROUPJOIN_NOT_TERMINAL, 
				packet.senderId, packet.groupid );
			return false;
		}

		pFrom->RemovePlayer(pFrom->GetNetId(), packet.senderId);
		pTo->AddPlayer(pTo->GetNetId(), packet.senderId);
		m_BasicProtocol.AckGroupJoin( pTo->GetNetId(), SEND_T_V, ERR_SUCCESS , packet.senderId, packet.groupid );
		m_BasicProtocol.AckGroupJoin( pFrom->GetNetId(), SEND_T_V, ERR_SUCCESS , packet.senderId, packet.groupid );
		return true;
	}
	else
	{ // Error!!
		m_BasicProtocol.AckGroupJoin( packet.senderId, SEND_TARGET, ERR_NOT_FOUND_GROUP, packet.senderId, packet.groupid );
		return false;
	}
}


//------------------------------------------------------------------------
// Request Create Group 
// 이미 group에 유저가 있으면 group이 자식으로 group을 생성할 수 없다.
// 만약 이렇게 하려면, group에 소속된 멤버들을 새 그룹에 소속시키고, 
// 현재 group의 자식으로 추가해야 한다. (단말 노드에만 유저가 소속될 수 있다.)
//------------------------------------------------------------------------
bool CBasicC2SHandler::ReqGroupCreate(basic::ReqGroupCreate_Packet &packet)
{
	if (!CheckRecvablePlayer(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher))
		return false;

	GroupPtr pParentGroup, pFrom, pNewGroup;
	if (!CreateBlankGroup(packet.senderId, packet.parentGroupId, packet.groupName, pParentGroup, pFrom, pNewGroup))
		return false;

	pFrom->RemovePlayer(pFrom->GetNetId(), packet.senderId);
	pNewGroup->AddPlayer(pNewGroup->GetNetId(), packet.senderId);
	pNewGroup->AddViewer( pParentGroup->GetNetId() );

	const netid groupId = pNewGroup->GetNetId();
	m_BasicProtocol.AckGroupCreate( pNewGroup->GetNetId(), SEND_T_V, ERR_SUCCESS, 
		packet.senderId, groupId, packet.parentGroupId, packet.groupName);
	m_BasicProtocol.AckGroupJoin( pNewGroup->GetNetId(), SEND_T_V, ERR_SUCCESS,
		packet.senderId, groupId);
	return true;
}


/**
 @brief Create Blank Group
 */
bool CBasicC2SHandler::ReqGroupCreateBlank(basic::ReqGroupCreateBlank_Packet &packet)
{
	if (!CheckRecvablePlayer(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher))
		return false;

	GroupPtr pParentGroup, pFrom, pNewGroup;
	if (!CreateBlankGroup(packet.senderId, packet.parentGroupId, packet.groupName, pParentGroup, pFrom, pNewGroup))
		return false;

	pNewGroup->AddViewer( pParentGroup->GetNetId() );
	m_BasicProtocol.AckGroupCreateBlank( pNewGroup->GetNetId(), SEND_T_V, ERR_SUCCESS, 
		packet.senderId, pNewGroup->GetNetId(), packet.parentGroupId, packet.groupName);
	return true;
}


/**
 @brief Create Blank Group
 
 이미 group에 유저가 있으면 group이 자식으로 group을 생성할 수 없다.
 만약 이렇게 하려면, group에 소속된 멤버들을 새 그룹에 소속시키고, 
 현재 group의 자식으로 추가해야 한다. 
 
 단말 노드에는 그룹을 추가할 수 없다.
 단말 노드에만 유저가 소속될 수 있다. 
 */
bool	CBasicC2SHandler::CreateBlankGroup( 
	netid senderId, const netid &parentGroupId, const std::string &groupName, 
	OUT GroupPtr &pParent, OUT GroupPtr &pFrom, OUT GroupPtr &pNew )
{
	GroupPtr pParentGroup = 
		(parentGroupId == INVALID_NETID)? &m_Server.GetRootGroup() : m_Server.GetRootGroup().GetChildandThis(parentGroupId);
	if (!pParentGroup)
	{ // Error!!
		m_BasicProtocol.AckGroupCreate( senderId, SEND_TARGET, 
			ERR_GROUPCREATE_NOT_FOUND_PARENT_GROUP, senderId, 0, parentGroupId, groupName );
		return false;
	}

	//if (pParentGroup->IsTerminal())
	//{// Error!!
	//	m_BasicProtocol.AckGroupCreate( senderId, SEND_TARGET, 
	//		ERR_GROUPCREATE_PARENT_TERMINALNODE, senderId, 0, parentGroupId, groupName );
	//	return false;
	//}

	GroupPtr pFromGroup = m_Server.GetRootGroup().GetChildFromPlayer( senderId );
	if (!pFromGroup)
	{ // Error!!
		m_BasicProtocol.AckGroupCreate( senderId, SEND_TARGET, ERR_NOT_FOUND_USER, 
			senderId, 0, parentGroupId, groupName );
		return false;
	}

	CGroup *pNewGroup = pParentGroup->AddChild( m_Server.GetGroupFactory() );
	//CGroup *pNewGroup = new CGroup(pParentGroup, groupName);
	//const bool result = pParentGroup->AddChild( pNewGroup );
	//if (!result) 
	if (!pNewGroup)
	{ // Error!!
		if (pParentGroup->IsTerminal())
		{
				m_BasicProtocol.AckGroupCreate( senderId, SEND_TARGET, 
					ERR_GROUPCREATE_PARENT_TERMINALNODE, senderId, 0, parentGroupId, groupName );
		}
		else
		{
			m_BasicProtocol.AckGroupCreate( senderId, SEND_TARGET, ERR_GROUPCREATE_NOR_MORE_CREATE_GROUP, 
				senderId, 0, parentGroupId, groupName );
		}
		return false;
	}

	pParent = pParentGroup;
	pFrom = pFromGroup;
	pNew = pNewGroup;
	return true;
}


/**
 @brief Request peer to peer connection
 */
bool CBasicC2SHandler::ReqP2PConnect(basic::ReqP2PConnect_Packet &packet)
{
	if (!CheckRecvablePlayer(&GetServer(), packet.senderId, &m_BasicProtocol, packet.pdispatcher))
		return false;

	// check p2p connection
	// if networking this group on p2p
	// -> then connect to p2p host with this senderId client
	// else
	// -> select p2p host in group client, and network p2p each other

	CSession* pClient = m_Server.GetSession(packet.senderId);
	if (!pClient)
	{
		clog::Error( clog::ERROR_PROBLEM, "not found Session netid: %d\n", packet.senderId );
		return false;
	}

	GroupPtr pGroup = m_Server.GetRootGroup().GetChildFromPlayer(packet.senderId);
	if (!pGroup)
	{
		clog::Error( clog::ERROR_PROBLEM, "not found group from user id: %d\n", packet.senderId );
		return false;
	}

	// search up and down p2p connection
	// if already connect p2p, then this group is p2p connection group
	const netid p2pHostClient = group::GetP2PHostClient(pGroup, CServerSessionAccess(&m_Server));
	if (INVALID_NETID == p2pHostClient)
	{
		const netid newHostClient = group::SelectP2PHostClient(pGroup);
		if (INVALID_NETID == newHostClient)
		{
			// error!!
			// maybe~ never happen this accident
			m_BasicProtocol.AckP2PConnect( packet.senderId, SEND_TARGET, 
				error::ERR_P2PCONNECTION_NO_MEMBER_IN_GROUP, network::P2P_CLIENT, " ", 0 );
			return false;
		}

		// p2p host 
		// build p2p network
		pClient->SetP2PState(P2P_HOST);
		pGroup->SetNetState(CGroup::NET_STATE_P2P);
		m_BasicProtocol.AckP2PConnect( packet.senderId, SEND_TARGET, error::ERR_SUCCESS, network::P2P_HOST, "", 2400 );
	}
	else
	{
		if (p2pHostClient == packet.senderId)
		{
			// error!!
			m_BasicProtocol.AckP2PConnect( packet.senderId, SEND_TARGET, 
				error::ERR_P2PCONNECTION_ALREADY_CONNECTED, network::P2P_CLIENT, "", 0);
			return false;
		}
		else
		{
			CSession *pHostClient = m_Server.GetSession(p2pHostClient);
			if (!pHostClient)
			{
				// error!!
				// maybe~ never happen this accident
				m_BasicProtocol.AckP2PConnect( packet.senderId, SEND_TARGET, 
					error::ERR_P2PCONNECTION_HOSTCLIENT_DISAPPEAR, network::P2P_CLIENT, "", 0);
				// waitting next command. maybe good work
				return false;
			}
			else
			{
				// connect p2p client
				m_BasicProtocol.AckP2PConnect( packet.senderId, SEND_TARGET, 
					error::ERR_SUCCESS, network::P2P_CLIENT, pHostClient->GetIp(), 2400);
			}
		}
	}

	return true;
}

