/**
Name:   Packet.h
Author:  jjuiddong
Date:    2012-11-28

��Ʈ��ũ ��ſ� ���̴� ��Ŷ�� �����Ѵ�.
��Ŷ ����

| protocol Id (4byte)| packet Id(4byte) | packet size(2byte) |  Data ~ |

// ������ PacketId
// 0 : Connect
// 1 : Disconnect
*/
#pragma once
#ifndef __PACKET_H__
#define __PACKET_H__

#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_array.hpp>

namespace network
{
	class CPacket
	{
	public:
		enum 
		{ 
			MAX_PACKETSIZE = 256, 
			MAX_STRINGSIZE = 128, //�ִ� ��Ʈ�� ������
			HEADER_SIZE = 10, // protocol id(4) + packet id(4) + packet size(2)
		};

		CPacket();
		CPacket(netid senderId, char *buf256);
		CPacket(const CPacket &rhs);
		virtual ~CPacket() {}

		void	EndPack(); /// call before send packet
		void	InitRead(); /// call before read

		template<class T> CPacket& operator<<(const T& rhs);
		template<class T> CPacket& operator>>(T& rhs);
		template<class T> void Append(const T &rhs);
		template<class T> void AppendPtr(const T &rhs, const size_t size);
		template<class T> void GetData(T &rhs);
		template<class T> void GetDataPtr(T &rhs, size_t size);
		void GetDataString( std::string &str);

		void			SetProtocolId(int protocolId);
		void			SetPacketId(int packetId);
		void			SetPacketSize(short packetSize);
		void			SetSenderId(netid id);
		int			GetProtocolId() const;
		int			GetPacketId() const;
		int			GetPacketSize() const;
		netid		GetSenderId() const;
		int			GetReadableSize() const;
		char*		GetData() const;

		CPacket& operator=(const CPacket &rhs);

	protected:
		short		GetPacketSizeFromRawData();

	protected:
		netid		m_SenderId;
		char			m_Data[ MAX_PACKETSIZE];
		int			m_ReadIdx;
		int			m_WriteIdx;
	};

	inline void	 CPacket::SetSenderId(netid id) { m_SenderId = id; }
	inline netid CPacket::GetSenderId() const { return m_SenderId; }
	inline char* CPacket::GetData() const { return (char*)m_Data; }
	inline int CPacket::GetPacketSize() const { return m_WriteIdx; }


	// �̷��� �Ǹ� ��� Ÿ���� binary ������ ����Ǳ� ������, ������ ȣ���� 
	// �ʿ��� �������� ������ ���� �� �ִ�.
	template<class T>
	inline CPacket& CPacket::operator<<(const T& rhs)
	{
		//const bool isPointer = boost::is_pointer<T>::type::value;
		//assert(!isPointer);
		//const bool isArray = boost::is_array<T>::type::value;
		//if (isArray)	AppendPtr(rhs, sizeof(T));
		//else			Append(rhs);
		Append(rhs);
		return *this;
	}

	template<class T>
	inline CPacket& CPacket::operator>>(T& rhs)
	{
		//const bool isPointer = boost::is_pointer<T>::type::value;
		//assert(!isPointer);
		//const bool isArray = boost::is_array<T>::type::value;
		//if (isArray)	GetDataPtr(rhs, sizeof(T));
		//else			GetData(rhs);
		GetData(rhs);
		return *this;
	}

	//------------------------------------------------------------------------
	// ��Ŷ�� m_WriteIdx���� ����Ÿ�� �����Ѵ�.
	//------------------------------------------------------------------------
	template<class T> 
	inline void CPacket::Append(const T &rhs)
	{
		if (m_WriteIdx+sizeof(T) >= MAX_PACKETSIZE)
			return;
		memmove_s(m_Data+m_WriteIdx, MAX_PACKETSIZE-m_WriteIdx, &rhs, sizeof(T));
		m_WriteIdx += sizeof(T);
	}
	template<class T> 
	inline void CPacket::AppendPtr(const T &rhs, const size_t size) // size : copy byte size
	{
		if (m_WriteIdx+size >= MAX_PACKETSIZE)
			return;
		memmove_s(m_Data+m_WriteIdx, MAX_PACKETSIZE-m_WriteIdx, rhs, size);
		m_WriteIdx += size;
	}

	//------------------------------------------------------------------------
	// ��Ŷ�� m_ReadIdx ���� ����Ÿ�� �����´�.
	//------------------------------------------------------------------------
	template<class T> 
	inline void CPacket::GetData(T &rhs)
	{
		if (m_ReadIdx+sizeof(T) >= MAX_PACKETSIZE)
			return;
		memmove_s(&rhs, sizeof(T), m_Data+m_ReadIdx, sizeof(T));
		m_ReadIdx += sizeof(T);
	}
	template<class T> 
	inline void CPacket::GetDataPtr(T &rhs, size_t size)
	{
		if (m_ReadIdx+size >= MAX_PACKETSIZE)
			return;
		memmove_s(rhs, size, m_Data+m_ReadIdx, size);
		m_ReadIdx += size;
	}
	inline void CPacket::GetDataString( std::string &str)
	{
		char buf[ CPacket::MAX_STRINGSIZE] = {NULL,};
		for (int i=0; i < MAX_STRINGSIZE-1 && (m_ReadIdx < MAX_PACKETSIZE); ++i)
		{
			buf[ i] = m_Data[ m_ReadIdx++];
			if (NULL == m_Data[ m_ReadIdx-1])
				break;
		}
		str = buf;
	}
	//_variant_t GetVariant(const _variant_t &varType);


	// Global Packet Functions
	enum RESERVED_PACKETID
	{
		PACKETID_CONNECT = 1,
		PACKETID_DISCONNECT,
		PACKETID_CLIENT_DISCONNECT,
		PACKETID_ACCEPT,
		PACKETID_P2P_MEMBER_JOIN,
		PACKETID_P2P_MEMBER_LEAVE,
	};

	CPacket DisconnectPacket(netid disconnectId, int netControllerUniqValue);
	CPacket ClientDisconnectPacket(netid disconnectId, int netControllerUniqValue);
	CPacket AcceptPacket(SOCKET acceptSocket, std::string clientIP );
	CPacket P2PMemberJoin(netid joinMemberId );
	CPacket P2PMemberLeave(netid leaveMemberId );

}

#endif // __PACKET_H__
