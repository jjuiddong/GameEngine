//------------------------------------------------------------------------
// Name:    ProtocolListener.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// 프로토콜을 받아 처리하는 역할을 한다.
//------------------------------------------------------------------------
#pragma once

namespace network
{
	class IProtocolListener
	{
	public:
		IProtocolListener() {}
		virtual ~IProtocolListener() {}
	public:
		virtual void BeforePacketProcess(CPacket &packet) {} // 패킷처리를 시작하기 전에 호출
		virtual void AfterPacketProcess(CPacket &packet) {} // 패킷을 처리한 후 호출된다.
	};
}
