//------------------------------------------------------------------------
// Name:    AllProtocol.h
// Author:  jjuiddong
// Date:    1/4/2013
// 
// ��� ���������� ��Ŷ�� ���� �� �ִ� ���������̴�.
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
