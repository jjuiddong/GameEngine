//------------------------------------------------------------------------
// Name:    MarshallingGroup.h
// Author:  jjuiddong
// Date:    3/11/2013
// 
// CGroup Marshalling
//------------------------------------------------------------------------
#pragma once

namespace network
{
	namespace marshalling
	{
		CPacket& operator<<(CPacket& packet, const CGroup &rhs);
		CPacket& operator>>(CPacket& packet, CGroup &rhs);
	}
}
