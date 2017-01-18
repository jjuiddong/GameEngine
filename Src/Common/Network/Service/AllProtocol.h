//------------------------------------------------------------------------
// Name:    AllProtocol.h
// Author:  jjuiddong
// Date:    1/4/2013
// 
// 모든 프로토콜의 패킷을 보낼 수 있는 프로토콜이다.
//------------------------------------------------------------------------
#pragma once

namespace all
{
	static const int Protocol_ID = -1;
	class Protocol : public network::IProtocol
	{
	public:
		Protocol() : IProtocol(Protocol_ID) {}
		void send(netid targetId, const network::SEND_FLAG flag, network::CPacket &packet);
	};
}
