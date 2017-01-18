
#include "stdafx.h"
#include "Group.h"
#include "../Interface/Factory.h"

using namespace network;


CGroup::CGroup(GroupPtr parent, const std::string &name)  : 
	m_Id(common::GenerateId())
,	m_Name(name)
,	m_pParent(parent)
,	m_NetState(NET_STATE_SERVERCLIENT)
{
	//m_Players.reserve(32);

}

// copy constructor
CGroup::CGroup(const CGroup &rhs)
{
	this->operator=(rhs);
}


CGroup::~CGroup() 
{
	Clear();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	CGroup::AddChild( CGroup *pGroup )
{
	RETV(!pGroup,false);

	GroupPtr ptr = GetChild(pGroup->GetNetId());
	if (ptr) return false; // already exist

	pGroup->SetParent(this);
	m_Children.insert(Groups::value_type(pGroup->GetNetId(), pGroup));
	
	// add group member
	BOOST_FOREACH(auto playerId, pGroup->GetPlayers())
	{
		AddPlayerNApplyParent(this, playerId);
	}
	return true;
}


/**
 @brief 비어있는 Group을 생성한다.

 이미 group에 유저가 있으면 group이 자식으로 group을 생성할 수 없다.
 만약 이렇게 하려면, group에 소속된 멤버들을 새 그룹에 소속시키고, 
 현재 group의 자식으로 추가해야 한다. 

 즉, 간단히 말하면,
 단말 노드에는 그룹을 추가할 수 없다.
 단말 노드에만 유저가 소속될 수 있다. 
 */
CGroup* CGroup::AddChild( IGroupFactory *pFactory )
{
	RETV(!pFactory, NULL);
	if (IsTerminal())
		return NULL;
	CGroup *pNewGroup = pFactory->New();
	if (!AddChild(pNewGroup))
	{
		delete pNewGroup;
		return NULL;
	}
	return pNewGroup;
}


//------------------------------------------------------------------------
// delete CGroup Object memory
//------------------------------------------------------------------------
bool	CGroup::RemoveChild( netid groupId )
{
	GroupPtr pGroup = GetChild(groupId);
	if (!pGroup) return false; // not exist

	// remove group member
	BOOST_FOREACH(auto playerId, m_Players)
	{
		RemovePlayerNApplyParent(pGroup->GetParent(), playerId);
	}

	GroupPtr parent = pGroup->GetParent();
	if (parent) // if parent is null then root group, root group never delete
	{
		auto it = parent->m_Children.find(groupId);
		if (parent->m_Children.end() == it)
			return false; // not exist member

		SAFE_DELETE(it->second);
		parent->m_Children.remove(groupId);	
	}
	return true;
}


//------------------------------------------------------------------------
// return child group object
//------------------------------------------------------------------------
GroupPtr CGroup::GetChild(netid groupId )
{
	auto it = m_Children.find( groupId );
	if (m_Children.end() != it)
		return GroupPtr(it->second);

	// search children
	BOOST_FOREACH(auto &child, m_Children.m_Seq)
	{
		GroupPtr ptr = child->GetChild(groupId);
		if (ptr) return ptr;
	}
	return NULL;
}


/**
 @brief return child or current of object same id
 */
GroupPtr	CGroup::GetChildandThis( netid groupId )
{
	if (GetNetId() == groupId)
		return this;
	return GetChild(groupId);
}


//------------------------------------------------------------------------
// return leaf group node object
//------------------------------------------------------------------------
GroupPtr CGroup::GetChildFromPlayer( netid playerId )
{
	if (!IsExistPlayer(playerId))
		return NULL;

	if (m_Children.empty()) // leaf node
		return this;

	// search children
	BOOST_FOREACH(auto &child, m_Children.m_Seq)
	{
		GroupPtr ptr = child->GetChildFromPlayer( playerId );
		if (ptr) return ptr;
	}

	return this;
}


/**
 @brief if this group is terminal node, return true or return false
 */
bool	CGroup::IsTerminal()
{
	if ((m_Children.size() <= 0) && (m_Players.size() > 0))
		return true;
	return false;
}


//------------------------------------------------------------------------
// 유저 추가
//------------------------------------------------------------------------
bool CGroup::AddPlayer(netid groupId, netid playerId)
{
	GroupPtr pGroup = (GetNetId() == groupId)? this : GetChild(groupId);
	if(!pGroup) return false; // not exist group
	return AddPlayerNApplyParent(pGroup, playerId);	
}


//------------------------------------------------------------------------
// 유저 제거
//------------------------------------------------------------------------
bool CGroup::RemovePlayer(netid groupId, netid playerId)
{
	GroupPtr pGroup = (GetNetId() == groupId)? this : GetChild(groupId);
	if(!pGroup) return false; // not exist group
	return RemovePlayerNApplyParent(pGroup, playerId);
}


//------------------------------------------------------------------------
// Player가 Group에 속해있다면 true를 리턴한다.
//------------------------------------------------------------------------
bool CGroup::IsExistPlayer(netid groupId, netid playerId)
{
	GroupPtr pGroup = GetChild(groupId);
	if(!pGroup) return false; // not exist group

	auto it = find(m_Players.begin(), m_Players.end(), playerId);
	return (m_Players.end() != it);
}


//------------------------------------------------------------------------
// 유저 추가
//------------------------------------------------------------------------
bool CGroup::AddPlayer(netid playerId)
{
	auto it = find(m_Players.begin(), m_Players.end(), playerId);
	if (m_Players.end() != it)
		return false; // 이미 존재한다면 실패
	m_Players.push_back( playerId );
	return true;
}


//------------------------------------------------------------------------
// Add playerId to Group Object 
// and Make Group Tree
//------------------------------------------------------------------------
bool	CGroup::AddPlayerNApplyParent(GroupPtr pGroup, netid playerId)
{
	if (!pGroup) 
		return true;
	if (!pGroup->AddPlayer(playerId))
		return false;
	return AddPlayerNApplyParent(pGroup->GetParent(), playerId);
}


//------------------------------------------------------------------------
// 유저 제거
//------------------------------------------------------------------------
bool CGroup::RemovePlayer(netid playerId)
{
	//auto it = find(m_Users.begin(), m_Users.end(), userId);
	//if (m_Users.end() == it)
	//	return false; // 없다면 실패
	//m_Users.erase(it);
	return common::removevector(m_Players, playerId);
}


//------------------------------------------------------------------------
// Remove playerId from Group Object
// and Apply Parent Group Node
//------------------------------------------------------------------------
bool	CGroup::RemovePlayerNApplyParent(GroupPtr pGroup, netid playerId)
{
	if (!pGroup) 
		return true;
	if (!pGroup->RemovePlayer(playerId))
		return false;
	return RemovePlayerNApplyParent(pGroup->GetParent(), playerId);
}


/**
 @brief Add Viewer
 */
bool	CGroup::AddViewer(netid id)
{
	auto it = find(m_Viewers.begin(), m_Viewers.end(), id);
	if (m_Viewers.end() != it)
		return false; // already exist, return
	m_Viewers.push_back( id );
	return true;
}


/**
 @brief Remove Viewer
 */
bool	CGroup::RemoveViewer(netid id)
{
	return common::removevector(m_Viewers, id);
}


//------------------------------------------------------------------------
// Player가 Group에 속해있다면 true를 리턴한다.
//------------------------------------------------------------------------
bool CGroup::IsExistPlayer(netid playerId)
{
	auto it = find(m_Players.begin(), m_Players.end(), playerId);
	return (m_Players.end() != it);
}


//------------------------------------------------------------------------
// 초기화
//------------------------------------------------------------------------
void CGroup::Clear()
{
	BOOST_FOREACH(auto &pGroup, m_Children.m_Seq)
	{
		SAFE_DELETE(pGroup);
	}
	m_Children.clear();
	m_Players.clear();
}


/**
 @brief 
 */
CGroup& CGroup::operator=(const CGroup &rhs)
{
	if (this != &rhs)
	{
		m_Id = rhs.m_Id;
		m_ParentId = rhs.m_ParentId;
		m_Name = rhs.m_Name;
		m_Tag = rhs.m_Tag;
		m_NetState = rhs.m_NetState;
		m_pParent = NULL;
		m_Players = rhs.m_Players;
		m_Viewers = rhs.m_Viewers;
	}
	return *this;
}
