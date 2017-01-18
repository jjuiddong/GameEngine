/**
Name:   BasicProtocolDispatcher.h
Author:  jjuiddong
Date:    3/21/2013

	�⺻ �������� ����
	����, Ŭ���̾�Ʈ Disconnect ��Ŷ�� �޴´�. �� ��Ŷ�� ���� ���μ��������� ������.
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
