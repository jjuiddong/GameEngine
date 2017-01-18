//------------------------------------------------------------------------
// Name:    ScriptDefine.h
// Author:  jjuiddong
// Date:    1/3/2013
// 
// �������� �Ľ̿� ���õ� ����Ʈ����, ��������� ��Ƴ��Ҵ�.
//------------------------------------------------------------------------
#pragma once

#include <string>

namespace network 
{
	enum
	{
		MAX_STRING = 256,
		MAX_ARGUMENT = 10,
	};

	enum Tokentype
	{
		_ERROR, ENDFILE, ID, NUM, FNUM, STRING, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, COMMA, COLON, SEMICOLON,
		PLUS, MINUS, TIMES, DIV, REMAINDER, REF, ARROW,
		LT/* < */, RT/* > */, LTEQ/* <= */, RTEQ/* >= */, NEQ/* != */, EQ/* == */, OR/* || */, AND/* && */, NEG/* ! */, SCOPE/*::*/,
		EVENT, UIEVENT, CUSTOMEVENT, IF, ELSE, WHILE, FUNCTION, ARG_IN, PROTOCOL,
	};

	enum NodeKind { Stmt, Exp };
	enum Kind
	{
		ProgramK, TutorialK, RoleTutorialK, TrainingK, PracticeK, StatementK, AIK, EventK, UIEventK, 
		CustomEventK, SuccessK, FailK, AssignK,
		ScriptWaitK, ScriptEndK, ScriptExitK, // statement
		TypeK, ConditionOpK, ConditionTreeK, OpK, IdK, FuncK, ParamK, ConstIntK, ConstFloatK, ConstStrK, CallK, // exp
	};

	enum { MAXCHILD=8, };

	enum eCONDITION_OP { OP_NONE, OP_AND, OP_OR, OP_NEG, OP_LT, OP_RT, OP_LTEQ, OP_RTEQ, OP_EQ, OP_NEQ };


	typedef struct _sTypeVar
	{
		std::string type;
		std::string var;

	} sTypeVar;

	typedef struct _sArg
	{
		sTypeVar *var;
		_sArg *next;
	} sArg;

	typedef struct _sProtocol
	{
		std::string name;
		sArg *argList;
		_sProtocol *next;
	} sProtocol;

	typedef struct _sRmi
	{
		std::string name;
		int number;
		sProtocol *protocol;
		_sRmi *next;
	} sRmi;


	// Release Protocol Parser Tree
	void ReleaseRmi(sRmi *p);
	void ReleaseRmiOnly(sRmi *p);
	void ReleaseProtocol(sProtocol *p);
	void ReleaseCurrentProtocol(sProtocol *p);
	void ReleaseArg(sArg *p);
	
	// Functions
	void PrintToken( Tokentype token, char *szTokenString );

	_variant_t	GetTypeStr2Type(const std::string &typeStr); // sArg->var->type
	//_variant_t	String2Variant(const std::string &valueType, const std::string &value);
	int			GetPacketID(sRmi *rmi, sProtocol *packet);
	std::string Packet2String(const CPacket &packet, sProtocol *protocol);

}

