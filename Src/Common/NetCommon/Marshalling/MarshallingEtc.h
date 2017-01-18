/**
Name:   MarshallingEtc.h
Author:  jjuiddong
Date:    4/18/2013

 ���� ��ƿ��Ƽ ��ü���� ������
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
