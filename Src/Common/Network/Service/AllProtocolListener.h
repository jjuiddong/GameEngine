//------------------------------------------------------------------------
// Name:    AllProtocolListener.h
// Author:  jjuiddong
// Date:    1/4/2013
// 
// 모든 프로토콜의 패킷을 받을 수 있는 프로토콜이다.
//------------------------------------------------------------------------
#pragma once

namespace all
{
	static const int Dispatcher_ID = -1;

	// Protocol Dispatcher
	class Dispatcher : public network::IProtocolDispatcher
	{
	public:
		Dispatcher() : IProtocolDispatcher(all::Dispatcher_ID) {}
		virtual bool Dispatch(network::CPacket &packet, const ProtocolListenerList &listeners) override;
	};


	// ProtocolListener
	class ProtocolListener : virtual public network::IProtocolListener
	{
		friend class Dispatcher;
		virtual void recv(netid senderId, network::CPacket &packet) {}
	};
}
