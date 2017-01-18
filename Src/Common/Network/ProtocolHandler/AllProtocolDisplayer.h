/**
Name:   AllProtocolDisplayer.h
Author:  jjuiddong
Date:    4/10/2013

*/
#pragma once

namespace network
{
	class AllProtocolDisplayer : public all::ProtocolListener
	{
	public:
		AllProtocolDisplayer(int displayType=0);
		virtual void recv(netid senderId, network::CPacket &packet) override;
	private:
		int m_DisplayType;

	};
}
