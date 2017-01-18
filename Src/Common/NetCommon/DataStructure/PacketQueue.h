/**
Name:   PacketQueue.h
Author:  jjuiddong
Date:    12/24/2012

 ��Ʈ��ũ�� ���� ���� ��Ŷ�� �����Ѵ�. ��� ������ Ŭ���̾�Ʈ�� �ϳ��� 
 ��Ŷť�� ����ȴ�.
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
			netid rcvNetId;	/// ��Ŷ�� ���� ������ netid
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
		PacketQueue	m_Packets;				/// ��Ʈ��ũ�κ��� ���� ��Ŷ�� �����Ѵ�.
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

	/// recvId �� �޴� ��Ŷ�� ��� �����Ѵ�.
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
