
#include "stdafx.h"
#include "MarshallingGroup.h"

using namespace network;
using namespace marshalling;


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
CPacket& marshalling::operator<<(CPacket& packet, const CGroup &rhs)
{
	packet << rhs.m_Id;
	packet << rhs.m_ParentId;
	packet << rhs.m_Name;
	packet << rhs.m_Players;
	packet << rhs.m_Tag;
	//packet << rhs.m_Children;

	return packet;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
CPacket& marshalling::operator>>(CPacket& packet, CGroup &rhs)
{
	packet >> rhs.m_Id;
	packet >> rhs.m_ParentId;
	packet >> rhs.m_Name;
	packet >> rhs.m_Players;
	packet >> rhs.m_Tag;
	//packet >> rhs.m_Children;

	return packet;
}
