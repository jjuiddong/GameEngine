
#include "stdafx.h"
#include "GroupTraverse.h"
#include "BindCore.h"
#include <boost/bind.hpp>

using namespace network;
using namespace group;



//------------------------------------------------------------------------
// search up and down tree node
//------------------------------------------------------------------------
bool	group::IsP2PConnection( GroupPtr pGroup )
{
	RETV(!pGroup, false);
	GroupPtr ptr = GetP2PGroup(pGroup);
	return (ptr? true : false);
}


//------------------------------------------------------------------------
// search up and down tree node 
// to find p2p host client
//------------------------------------------------------------------------
netid group::GetP2PHostClient( GroupPtr pGroup, ISessionAccess &userAccess)
{
	GroupPtr p2pGroup = GetP2PGroup(pGroup);
	if (!p2pGroup)
		return INVALID_NETID;

	const NetIdes &users = p2pGroup->GetPlayers();
	BOOST_FOREACH(auto &userId, users)
	{
		SessionPtr clientPtr = userAccess.GetSession(userId);
		if (!clientPtr)
			continue;
		if (P2P_HOST == clientPtr->GetP2PState())
			return userId;				
	}
	return INVALID_NETID;
}


//------------------------------------------------------------------------
// return p2p group
// search up and down tree node to find p2p group
// p2p group is only one node in tree line
//------------------------------------------------------------------------
GroupPtr group::GetP2PGroup( GroupPtr pGroup )
{
	RETV(!pGroup, NULL);
	if (pGroup->GetNetState() == CGroup::NET_STATE_P2P)
		return pGroup;

	GroupPtr ptr = TraverseUpward( pGroup, 
		boost::bind(&IsSameValue<CGroup::NET_STATE>,
			boost::bind(&CGroup::GetNetState, _1), CGroup::NET_STATE_P2P) );
	if (ptr)
		return ptr;

	ptr = TraverseDownward( pGroup, 
		boost::bind(&IsSameValue<CGroup::NET_STATE>,
			boost::bind(&CGroup::GetNetState, _1), CGroup::NET_STATE_P2P) );
	if (ptr)
		return ptr;

	return NULL;
}


/**
 @brief Select P2P Host Client In Group Member
*/
netid group::SelectP2PHostClient( GroupPtr pGroup )
{
	RETV(!pGroup, INVALID_NETID);

	if (pGroup->GetPlayers().size() <= 0)
		return INVALID_NETID;

	return pGroup->GetPlayers().front();
}
