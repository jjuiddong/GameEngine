
#include "stdafx.h"
#include "Packet.h"
#include "NetCommon/Marshalling/Marshalling.h"
//#include "../Controller/NetController.h"


using namespace network;
using namespace marshalling;
/**
@brief  Disconnect 패킷을 리턴한다.
@param netid: disconnect 된 netid
*/
CPacket network::DisconnectPacket(netid disconnectId, int netControllerUniqValue)
{
	CPacket packet;
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_DISCONNECT);
	packet << netControllerUniqValue; // packet 을 검증하기 위한 용도로 쓰인다.
	packet << disconnectId;
	return packet;
}


/**
 @brief Client Disconnect 패킷을 리턴한다.
 @param netid: disconnect 된 netid
 */
CPacket network::ClientDisconnectPacket(netid disconnectId, int netControllerUniqValue)
{
	CPacket packet;
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_CLIENT_DISCONNECT);
	packet << netControllerUniqValue; // packet 을 검증하기 위한 용도로 쓰인다.
	packet << disconnectId;
	return packet;
}


/**
 @brief return the AcceptPacket
 */
CPacket network::AcceptPacket(SOCKET acceptSocket, std::string clientIP )
{
	CPacket packet;
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_ACCEPT);
	packet << acceptSocket;
	packet << clientIP;
	return packet;
}


/**
 @brief return the Member Join Packet
 */
CPacket network::P2PMemberJoin(netid joinMemberId )
{
	CPacket packet;
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_P2P_MEMBER_JOIN);
	packet << joinMemberId;
	return packet;
}


/**
 @brief return the Member Leave Packet
 */
CPacket network::P2PMemberLeave(netid leaveMemberId )
{
	CPacket packet;
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_P2P_MEMBER_LEAVE);
	packet << leaveMemberId;
	return packet;
}


CPacket::CPacket():
	m_WriteIdx(HEADER_SIZE)
,	m_ReadIdx(HEADER_SIZE)
{
}

CPacket::CPacket(const CPacket &rhs)
{
	CPacket::operator=(rhs);
}

CPacket::CPacket(netid senderId, char *buf256) :
	m_SenderId(senderId)
,	m_WriteIdx(HEADER_SIZE)
,	m_ReadIdx(HEADER_SIZE)
{
	if (buf256) // todo: packet size copy
	{
		memcpy( m_Data, buf256, MAX_PACKETSIZE);
		m_WriteIdx = GetPacketSizeFromRawData();
	}
}


/**
	@brief 
	*/
CPacket& CPacket::operator=(const CPacket &rhs)
{
	if (this != &rhs)
	{
		m_SenderId = rhs.m_SenderId;
		memcpy(m_Data, rhs.m_Data, sizeof(rhs.m_Data));
		m_ReadIdx = rhs.m_ReadIdx;
		m_WriteIdx = rhs.m_WriteIdx;
	}
	return *this;
}


/**
	@brief 
	*/
void	CPacket::SetProtocolId(int protocolId)
{
	*(int*)m_Data = protocolId;
}


/**
 @brief 
 */
void	CPacket::SetPacketId(int packetId)
{
	*(int*)(m_Data+sizeof(int)) = packetId;
}


/**
 @brief 
 */
void	CPacket::SetPacketSize(short packetSize)
{
	*(short*)(m_Data+sizeof(int)+sizeof(int)) = packetSize;
}


//------------------------------------------------------------------------
// 패킷의 프로토콜 정보를 리턴한다.
//------------------------------------------------------------------------
int	CPacket::GetProtocolId() const
{
	const int id = *(int*)m_Data;
	return id;
}


//------------------------------------------------------------------------
// 패킷의 패킷 아이디 정보를 리턴한다.
//------------------------------------------------------------------------
int	CPacket::GetPacketId() const 
{
	const int id = *(int*)(m_Data+sizeof(int));
	return id;
}


/**
 @brief Data 에 저장된 packet size 값을 리턴한다. 일반적으로 m_WirteIdx를 
 리턴하는게 보통이다.
 */
short CPacket::GetPacketSizeFromRawData()
{
	const short size = *(short*)(m_Data+sizeof(int)+sizeof(int));
	return size;
}


/**
 @brief  읽을 수 있는 패킷 사이즈를 리턴한다. (byte단위)
 */
int	CPacket::GetReadableSize() const
{
	return GetPacketSize() - m_ReadIdx;
}


/**
 @brief save packet size
 */
void	CPacket::EndPack()
{
	SetPacketSize( (short)GetPacketSize() );
}


/**
 @brief 
 */
void	CPacket::InitRead()
{
	m_ReadIdx = HEADER_SIZE;
}


/**
 @brief 
 */
//int	CPacket::GetPacketSize() const
//{
//	const int size = *(int*)(m_Data+sizeof(int)+sizeof(int));
//	return size;
//}


//CPacket& CPacket::operator<<(const _variant_t &rhs)
//{
//	const int type = rhs.vt;
//	switch (type)
//	{
//	case VT_I2: Append(rhs.iVal); break;
//	case VT_I4: Append(rhs.lVal); break;
//	case VT_R4: Append(rhs.fltVal); break;
//	case VT_R8: Append(rhs.dblVal); break;
//
//	case VT_BSTR:
//		{
//			tstring str = (LPCTSTR) (_bstr_t)rhs.bstrVal;
//#ifdef _UNICODE
//			std::string s = common::wstr2str(str);
//			operator<<(s);
//#else
//			operator<<(str);
//#endif
//		}
//		break;
//
//	case VT_DECIMAL:
//	case VT_I1:
//	case VT_UI1:
//	case VT_UI2:
//	case VT_UI4:
//		break;
//
//	case VT_INT: Append(rhs.intVal); break;
//	case VT_UINT: Append(rhs.uintVal); break;
//	default:
//		{
//			error::ErrorLog( 
//				common::format("CPacket::operator<< %d 에 해당하는 타입의 Append는 없다.", type) );
//			assert(0);
//		}
//		break;
//	}
//	return *this;
//}


//		VT_EMPTY	= 0,
//			VT_NULL	= 1,
// 			VT_CY	= 6,
// 			VT_DATE	= 7,

// 			VT_DISPATCH	= 9,
// 			VT_ERROR	= 10,
// 			VT_BOOL	= 11,
// 			VT_VARIANT	= 12,
// 			VT_UNKNOWN	= 13,
// 			VT_I8	= 20,
// 			VT_UI8	= 21,


// 			VT_VOID	= 24,
// 			VT_HRESULT	= 25,
// 			VT_PTR	= 26,
// 			VT_SAFEARRAY	= 27,
// 			VT_CARRAY	= 28,
// 			VT_USERDEFINED	= 29,
// 			VT_LPSTR	= 30,
// 			VT_LPWSTR	= 31,
// 			VT_RECORD	= 36,
// 			VT_INT_PTR	= 37,
// 			VT_UINT_PTR	= 38,
// 			VT_FILETIME	= 64,
// 			VT_BLOB	= 65,
// 			VT_STREAM	= 66,
// 			VT_STORAGE	= 67,
// 			VT_STREAMED_OBJECT	= 68,
// 			VT_STORED_OBJECT	= 69,
// 			VT_BLOB_OBJECT	= 70,
// 			VT_CF	= 71,
// 			VT_CLSID	= 72,
// 			VT_VERSIONED_STREAM	= 73,
// 			VT_BSTR_BLOB	= 0xfff,
// 			VT_VECTOR	= 0x1000,
// 			VT_ARRAY	= 0x2000,
// 			VT_BYREF	= 0x4000,
// 			VT_RESERVED	= 0x8000,
// 			VT_ILLEGAL	= 0xffff,
// 			VT_ILLEGALMASKED	= 0xfff,
// 			VT_TYPEMASK	= 0xfff


////------------------------------------------------------------------------
//// 인자로 넘어온 varType 타입으로 패킷에 있는 데이타를 가져와 리턴한다.
////------------------------------------------------------------------------
//_variant_t CPacket::GetVariant(const _variant_t &varType)
//{
//	_variant_t var = varType;
//	switch (varType.vt)
//	{
//	case VT_I2: *this >>(var.iVal); break;
//	case VT_I4: *this  >>(var.lVal); break;
//	case VT_R4: *this  >>(var.fltVal); break;
//	case VT_R8: *this  >>(var.dblVal); break;
//
//	case VT_BSTR:
//		{
//			std::string str;
//			*this  >>(str);
// #ifdef _UNICODE
// 			var.bstrVal = (_bstr_t)common::str2wstr(str).c_str();
// #else
// 			var.bstrVal = (_bstr_t)str.c_str();
// #endif
//		}
//		break;
//
//	case VT_DECIMAL:
//	case VT_I1:
//	case VT_UI1:
//	case VT_UI2:
//	case VT_UI4:
//		break;
//
//	case VT_INT: *this  >>(var.intVal); break;
//	case VT_UINT: *this >>(var.uintVal); break;
//	default:
//		break;
//	}
//	return var;
//}
