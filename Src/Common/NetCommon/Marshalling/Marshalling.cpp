
#include "stdafx.h"
#include "Marshalling.h"

using namespace network;
using namespace marshalling;


//------------------------------------------------------------------------
//  << 
//------------------------------------------------------------------------
//CPacket& marshalling::operator<<(CPacket& packet, const bool &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<(CPacket& packet, const long &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<(CPacket& packet, const int &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<( CPacket& packet, const unsigned int &rhs)
//{
//	packet.Append( rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<( CPacket& packet, const char &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<( CPacket& packet, const unsigned char &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<( CPacket& packet, const float &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<( CPacket& packet, const double &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<( CPacket& packet, const short &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
//CPacket& marshalling::operator<<( CPacket& packet, const unsigned short &rhs)
//{
//	packet.Append(rhs);
//	return packet;
//}
// packet member 접근이 많은 코드다. 수정이 필요하다.
CPacket& marshalling::operator<<(CPacket& packet, const std::string &rhs)
{
	const int length = rhs.size();
	packet.AppendPtr(rhs.c_str(), length+1);
	return packet;
}
CPacket& marshalling::operator<<(CPacket& packet, const _variant_t &rhs)
{
	const int type = rhs.vt;
	switch (type)
	{
	case VT_I2: packet.Append(rhs.iVal); break;
	case VT_I4: packet.Append(rhs.lVal); break;
	case VT_R4: packet.Append(rhs.fltVal); break;
	case VT_R8: packet.Append(rhs.dblVal); break;

	case VT_BOOL: packet.Append(rhs.bVal); break;
	case VT_DECIMAL: break;
	case VT_I1: packet.Append(rhs.cVal); break;
	case VT_UI1: packet.Append(rhs.bVal); break;
	case VT_UI2: packet.Append(rhs.uiVal); break;
	case VT_UI4: packet.Append(rhs.ulVal); break;
	case VT_I8: packet.Append(rhs.llVal); break;
	case VT_UI8: packet.Append(rhs.ullVal); break;

	case VT_INT: packet.Append(rhs.intVal); break;
	case VT_UINT: packet.Append(rhs.uintVal); break;

	case VT_BSTR:
		{
			tstring str = (LPCTSTR) (_bstr_t)rhs.bstrVal;
#ifdef _UNICODE
			std::string s = common::wstr2str(str);
			packet<<(s);
#else
			packet<<(str);
#endif
		}
		break;

	default:
		{
			clog::Error( clog::ERROR_PROBLEM, 
				common::format("marshalling::operator<< %d 에 해당하는 타입의 Append는 없다.\n", type) );
			assert(0);
		}
		break;
	}
	return packet;
}
CPacket& marshalling::operator<<(CPacket& packet, const P2P_STATE &rhs)
{
	AppendEnum(packet, rhs);
	return packet;
}


//------------------------------------------------------------------------
// >>
//------------------------------------------------------------------------
//CPacket& marshalling::operator>>(CPacket& packet, bool &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, long &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, int &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, unsigned int &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, char &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, unsigned char &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, float &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, double &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, short &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
//CPacket& marshalling::operator>>(CPacket& packet, unsigned short &rhs)
//{
//	packet.GetData(rhs);
//	return packet;
//}
CPacket& marshalling::operator>>(CPacket& packet, std::string &rhs)
{
	packet.GetDataString(rhs);
	return packet;
}
CPacket& marshalling::operator>>(CPacket& packet, P2P_STATE &rhs)
{
	GetDataEnum(packet, rhs);
	return packet;
}


//------------------------------------------------------------------------
// 인자로 넘어온 varType 타입 값으로 패킷에 있는 데이타를 가져와 리턴한다.
//------------------------------------------------------------------------
CPacket& marshalling::operator>>(CPacket& packet, _variant_t &varType)
{
	switch (varType.vt)
	{
	case VT_I2: packet >> varType.iVal; break;
	case VT_I4: packet  >> varType.lVal; break;
	case VT_R4: packet  >> varType.fltVal; break;
	case VT_R8: packet  >> varType.dblVal; break;

	case VT_BOOL: packet >> varType.bVal; break;
	case VT_DECIMAL: break;
	case VT_I1: packet >> varType.cVal; break;
	case VT_UI1: packet >> varType.bVal; break;
	case VT_UI2: packet >> varType.uiVal; break;
	case VT_UI4: packet >> varType.ulVal; break;
	case VT_I8: packet >> varType.llVal; break;
	case VT_UI8: packet >> varType.ullVal; break;

	case VT_INT: packet  >>(varType.intVal); break;
	case VT_UINT: packet  >>(varType.uintVal); break;

	case VT_BSTR:
		{
			std::string str;
			packet  >>(str);
#ifdef _UNICODE
			varType.bstrVal = (_bstr_t)common::str2wstr(str).c_str();
#else
			varType.bstrVal = (_bstr_t)str.c_str();
#endif
		}
		break;

	default:
		break;
	}
	return packet ;
}
