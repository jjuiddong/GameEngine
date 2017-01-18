//------------------------------------------------------------------------
// Name:    ProtocolParser.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// 프로토콜 파서
//------------------------------------------------------------------------
#pragma once

#include "ProtocolScanner.h"

namespace network
{
	class CProtocolScanner;
	class CProtocolParser
	{
	public:
		CProtocolParser();
		virtual ~CProtocolParser();

		sRmi* Parse( const char *szFileName, BOOL bTrace=FALSE );
		BOOL Parse( BYTE *pFileMem, int nFileSize , BOOL bTrace=FALSE );
		BOOL IsError() { return m_bError; }
		void SetAutoRemove(BOOL bAutoRemove) { m_bAutoRemove = bAutoRemove; }
		void Clear();


	private:
		BOOL Match( Tokentype t );
		void SyntaxError( char *szMsg, ... );

		void WritePIDLMacro(std::string PIDLFileName, sRmi *p);
		void WriteRmi(FILE *fp, sRmi *p);
		void WriteProtocol(FILE *fp, sRmi *rmi, sProtocol *p);
		void WriteFirstArg(FILE *fp, sArg*p);
		void WriteArg(FILE *fp, sArg*p, bool isComma);
		void WriteFirstArgVar(FILE *fp, sArg*p);
		void WriteArgVar(FILE *fp, sArg*p, bool isComma);


		// expr -> rmi_list
		// rmi_list -> (rmi)*
		// rmi -> protocol id number '{' stmt_list '}'
		// stmt_list -> (stmt)*
		// stmt -> protocol semicolon
		// protocol -> id '(' arg_list ')'
		// arg_list -> [arg (',' arg)*]
		// arg -> type
		// type -> type_sub (var)?
		// type_sub -> id '<' type_sub '>'
		//			| id::id
		//			| id
		// var -> '*' id (index)?
		//	    | '&' id (index)?
		//		| id (index)?
		//	    | '*'
		//		| '&'
		// index -> '[' (number)? ']'

		sRmi* rmi_list();
		sRmi* rmi();
		sProtocol* stmt_list();
		sProtocol* stmt();
		sProtocol* protocol();
		sArg* arg_list();
		sArg* arg();
		sTypeVar* type();
		std::string type_sub();
		std::string var();
		std::string index();
		std::string number();
		int num();
		std::string id();

	private:
		CProtocolScanner *m_pScan;
		sRmi	 *m_pRmiList;
		char m_FileName[ MAX_PATH];
		Tokentype m_Token;
		BOOL m_bTrace;
		BOOL m_bError;
		BOOL m_bAutoRemove;

	};
}