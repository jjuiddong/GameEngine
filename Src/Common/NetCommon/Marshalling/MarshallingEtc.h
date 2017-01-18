/**
Name:   MarshallingEtc.h
Author:  jjuiddong
Date:    4/18/2013

 각종 유틸리티 객체들의 마샬링
*/
#pragma once

namespace network
{

	struct SHostInfo;

	namespace marshalling
	{
		// SHostInfo
		CPacket& operator<<(CPacket& packet, const SHostInfo &rhs);
		CPacket& operator>>(CPacket& packet, SHostInfo &rhs);


	}
}
