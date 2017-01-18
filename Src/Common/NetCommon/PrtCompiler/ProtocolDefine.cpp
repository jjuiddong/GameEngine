
#include "stdafx.h"
#include "ProtocolDefine.h"
#include "../Error/ErrorCode.h"


using namespace network;
using namespace marshalling;

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void network::PrintToken( Tokentype token, char *szTokenString )
{
	switch( token )
	{
	case ID:
	case STRING:	printf( "string = %s\n", szTokenString ); break;

	case ASSIGN:	printf( "=" ); break;
	case TIMES:		printf( "*" ); break;
	case LPAREN:	printf( "(" ); break;
	case RPAREN:	printf( ")" ); break;
	case LBRACE:	printf( "{" ); break;
	case RBRACE:	printf( "}" ); break;
	case COMMA:	printf( "," ); break;
	case NUM:		printf( "number" ); break;
	case OR:		printf( "||" ); break;
	case AND:		printf( "&&" ); break;

	default: 		printf( "UnKnown token %d, %s\n", token, szTokenString ); break;
	}
}


//------------------------------------------------------------------------
// typeStr: sArg->var->type
// 스트링을 타입으로 리턴한다.
//------------------------------------------------------------------------
_variant_t network::GetTypeStr2Type(const std::string &typeStr)
{
	if (typeStr == "std::string")
	{
		char *v=NULL;
		return _variant_t(v);
	}
	else if (typeStr == "string")
	{
		char *v=NULL;
		return _variant_t(v);
	}
	else if (typeStr == "float")
	{
		float v=0.f;
		return _variant_t(v);
	}
	else if (typeStr == "double")
	{
		double v=0.f;
		return _variant_t(v);
	}
	else if (typeStr == "int")
	{
		int v=0;
		return _variant_t(v);
	}
	else if (typeStr == "char")
	{
		char v='a';
		return _variant_t(v);
	}
	else if (typeStr == "short")
	{
		short v=0;
		return _variant_t(v);
	}
	else if (typeStr == "long")
	{
		long v=0;
		return _variant_t(v);
	}
	else if (typeStr == "bool")
	{
		bool v=true;
		return _variant_t(v);
	}
	else if (typeStr == "BOOL")
	{
		bool v=true;
		return _variant_t(v);
	}

	return _variant_t(1);
}


//------------------------------------------------------------------------
// 인자로 넘어온 value 스트링을 valueType 스트링 타입으로 변환해서 리턴시킨다.
//------------------------------------------------------------------------
// _variant_t	network::String2Variant(const std::string &valueType, const std::string &value)
// {
// 	_variant_t varType = GetTypeStr2Type(valueType);
// 	_variant_t var = varType;
// 	switch (varType.vt)
// 	{
// 	case VT_I2: var.iVal = (short)atoi(value.c_str()); break;
// 	case VT_I4: var.lVal = (long)atoi(value.c_str()); break;
// 	case VT_R4: var.fltVal = (float)atof(value.c_str()); break;
// 	case VT_R8: var.dblVal = atof(value.c_str()); break;
// 
// 	case VT_BSTR:
// 		{
// #ifdef _UNICODE
// 			var.bstrVal = (_bstr_t)common::string2wstring(value).c_str();
// #else
// 			var.bstrVal = (_bstr_t)value.c_str();
// #endif
// 		}
// 		break;
// 
// 	case VT_DECIMAL:
// 	case VT_I1:
// 	case VT_UI1:
// 	case VT_UI2:
// 	case VT_UI4:
// 		break;
// 
// 	case VT_INT: var.intVal = (int)atoi(value.c_str()); break;
// 	case VT_UINT: var.uintVal = strtoul(value.c_str(),NULL,0); break;
// 	default:
// 		break;
// 	}
// 
// 	return var;
// }


//------------------------------------------------------------------------
// packetID를 리턴한다. 여기서 Packet이란 sProtocol protocol을 의미하고, 
// sRmi 의 자식으로 순서대로 번호가 매겨진 값이 Packet ID이다.
//------------------------------------------------------------------------
int	network::GetPacketID(sRmi *rmi, sProtocol *packet)
{
	if (!rmi) return 0;
	if (!packet) return rmi->number;

	int id = rmi->number + 1;
	sProtocol *p = rmi->protocol;
	while (p)
	{
		if (p == packet)
			break;
		++id;
		p = p->next;
	}
	return id;
}


//------------------------------------------------------------------------
// 패킷내용을 스트링으로 변환 한다.
//------------------------------------------------------------------------
std::string network::Packet2String(const CPacket &packet, sProtocol *protocol)
{
	if (!protocol)
	{
		return format( " protocol id = %d, packet id = %d", packet.GetProtocolId(), packet.GetPacketId() );
	}

	std::stringstream ss;
	CPacket tempPacket = packet;
	tempPacket.InitRead();

	const int protocolID = tempPacket.GetProtocolId();
	const int packetID = tempPacket.GetPacketId();

	ss << protocol->name << " sender = " << tempPacket.GetSenderId() << " ";

	sArg *arg = protocol->argList;
	while (arg)
	{
		//const _variant_t varType = GetTypeStr2Type(arg->var->type);
		//const _variant_t var = tempPacket.GetVariant(varType);
		_variant_t var = GetTypeStr2Type(arg->var->type);
		tempPacket >> var;

		ss << arg->var->var + " = ";
		ss << common::variant2str(var);
		if ( arg->var->var == "errorCode")
			ss << "(" << ErrorCodeString((error::ERROR_CODE)(int)var) << ")";

		ss << ", ";

		arg = arg->next;
	}

	return ss.str();
}


/**
 @brief 
 */
void network::ReleaseRmi(sRmi *p)
{
	if (!p) return;
	ReleaseProtocol(p->protocol);
	ReleaseRmi(p->next);
	delete p;
}


/**
 @brief ReleaseRmiOnly
 */
void network::ReleaseRmiOnly(sRmi *p)
{
	if (!p) return;
	ReleaseRmiOnly(p->next);
	delete p;
}

/**
 @brief 
 */
void network::ReleaseProtocol(sProtocol *p)
{
	if (!p) return;
	ReleaseArg(p->argList);
	ReleaseProtocol(p->next);
	delete p;
}


/**
 @brief 
 */
void network::ReleaseCurrentProtocol(sProtocol *p)
{
	if (!p) return;
	ReleaseArg(p->argList);
	delete p;
}


/**
 @brief 
 */
void network::ReleaseArg(sArg *p)
{
	if (!p) return;
	if (p->var) delete p->var;
	ReleaseArg(p->next);
	delete p;
}