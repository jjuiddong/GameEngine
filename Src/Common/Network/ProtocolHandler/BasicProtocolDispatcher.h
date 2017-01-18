/**
Name:   BasicProtocolDispatcher.h
Author:  jjuiddong
Date:    3/21/2013

	기본 프로토콜 정의
	서버, 클라이언트 Disconnect 패킷을 받는다. 이 패킷은 내부 프로세스에서만 보낸다.
*/
#pragma once

namespace basic_protocol
{
	static const int Dispatcher_ID = 0;

	// Server Basic Protocol Dispatcher
	class ServerDispatcher
	{
	public:
		void Dispatch(network::CPacket &packet, ServerBasicPtr pSvr);
	};

	// Server Basic Protocol Dispatcher
	class ClientDispatcher
	{
	public:
		void Dispatch(network::CPacket &packet, CoreClientPtr pClt);
	};	
}
