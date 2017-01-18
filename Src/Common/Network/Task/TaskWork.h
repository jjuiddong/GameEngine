//------------------------------------------------------------------------
// Name:    TaskWork.h
// Author:  jjuiddong
// Date:    12/22/2012
// 
// 하나의 소켓과 통신하는 기능을 한다.
//------------------------------------------------------------------------
#pragma once


namespace network
{
	DECLARE_TYPE_NAME_SCOPE(network, CTaskWork)
	class CTaskWork : public common::CTask
		, public memmonitor::Monitor<CTaskWork, TYPE_NAME(network::CTaskWork)>
	{
	public:
		CTaskWork(int taskId, netid netId, SOCKET sock);
		virtual RUN_RESULT	Run() override;

	protected:
		netid	m_NetId;
		SOCKET m_Socket;
	};


	inline CTaskWork::CTaskWork(int taskId, netid netId, SOCKET sock) : 
		CTask(taskId,"TaskWork"), m_NetId(netId), m_Socket(sock) 
	{
	}

	//------------------------------------------------------------------------
	// Run
	//------------------------------------------------------------------------
	inline common::CTask::RUN_RESULT	CTaskWork::Run()
	{
		const timeval t = {0, 0}; // 0 millisecond
		fd_set readSockets;
		FD_ZERO(&readSockets);
		FD_SET(m_Socket, &readSockets);

		const int ret = select( readSockets.fd_count, &readSockets, NULL, NULL, &t );
		if (ret != 0 && ret != SOCKET_ERROR)
		{
			char buf[ CPacket::MAX_PACKETSIZE];
			const int result = recv( m_Socket, buf, sizeof(buf), 0);
			if (result == SOCKET_ERROR || result == 0) // 받은 패킷사이즈가 0이면 서버와 끊겼다는 의미다.
			{
				CPacketQueue::Get()->PushPacket( 
					CPacketQueue::SPacketData(m_NetId, 
						DisconnectPacket(m_NetId, CController::Get()->GetUniqueValue()) ));
				return RR_END;
			}
			else
			{
				CPacketQueue::Get()->PushPacket( 
					CPacketQueue::SPacketData(m_NetId, CPacket(SERVER_NETID,buf)) );
			}
		}
		return RR_CONTINUE;
	}

}
