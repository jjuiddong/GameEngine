/**
Name:   PacketQueue.h
Author:  jjuiddong
Date:    12/24/2012

 네트워크를 통해 받은 패킷을 저장한다. 모든 서버와 클라이언트는 하나의 
 패킷큐에 저장된다.
*/
#pragma once

namespace network
{
	class CPacketQueue : public common::CSingleton<CPacketQueue>
	{
	public:
		friend class CTaskAccept;
		friend class CTaskLogic;
		friend class CTaskWork;
		friend class CTaskWorkClient;
		friend class CTaskWorkServer;
		friend class CController;
		friend class CCoreClient;
		friend class CServerBasic;

		struct SPacketData
		{
			netid rcvNetId;	/// 패킷을 받은 서버의 netid
			CPacket packet;
			SPacketData() {}
			SPacketData(netid _rcvNetId, const CPacket &p) : rcvNetId(_rcvNetId), packet(p) {}
		};

	protected:
		void PushPacket(const SPacketData &data);
		bool PopPacket(OUT SPacketData &data);
		bool PopPacket(netid recvId, OUT SPacketData &data);
		bool RemovePacket( netid recvId );

	protected:
		typedef std::list<SPacketData> PacketQueue;
		typedef PacketQueue::iterator PacketItor;
		PacketQueue	m_Packets;				/// 네트워크로부터 받은 패킷을 저장한다.
		common::CriticalSection	m_CS;
	};


	/// Push Packet 
	inline void CPacketQueue::PushPacket(const SPacketData &data)
	{
		common::AutoCSLock cs(m_CS);
		m_Packets.push_back(data);
	}

	/// Pop Packet
	inline bool CPacketQueue::PopPacket(OUT SPacketData &data)
	{
		common::AutoCSLock cs(m_CS);
		if (m_Packets.empty())
			return false;
		data = m_Packets.front();
		m_Packets.pop_front();
		return true;
	}

	/// Pop Packet whose recvId
	inline bool CPacketQueue::PopPacket(netid recvId, OUT SPacketData &data)
	{
		common::AutoCSLock cs(m_CS);
		if (m_Packets.empty())
			return false;

		PacketItor it = m_Packets.begin();
		while (m_Packets.end() != it)
		{
			if (it->rcvNetId == recvId)
			{
				data = *it;
				m_Packets.erase(it);
				return true;
			}
			++it;
		}
		return false;
	}

	/// recvId 가 받는 패킷을 모두 제거한다.
	inline bool CPacketQueue::RemovePacket( netid recvId )
	{
		common::AutoCSLock cs(m_CS);
		if (m_Packets.empty())
			return false;

		PacketItor it = m_Packets.begin();
		while (m_Packets.end() != it)
		{
			if (it->rcvNetId == recvId)
			{
				it = m_Packets.erase(it);
			}
			else
			{
				++it;
			}			
		}
		return true;
	}

}
