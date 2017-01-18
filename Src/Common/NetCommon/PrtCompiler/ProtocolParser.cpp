

#include "stdafx.h"
#include "ProtocolParser.h"

using namespace network;

CProtocolParser::CProtocolParser() 
{
	m_pScan = new CProtocolScanner();
	m_pRmiList = NULL;
	m_bTrace = FALSE;
	m_bError = FALSE;
	m_bAutoRemove = TRUE;

}

CProtocolParser::~CProtocolParser()
{
	SAFE_DELETE(m_pScan);

	if (m_bAutoRemove)
	{
		ReleaseRmi(m_pRmiList);
	}
}


//---------------------------------------------------------------------
// Ʃ�丮�� ��ũ��Ʈ�� �Ľ��Ѵ�.
//---------------------------------------------------------------------
sRmi* CProtocolParser::Parse( const char *szFileName, BOOL bTrace )
{
	if( !m_pScan->LoadFile(szFileName, bTrace) )
		return NULL;

	strcpy_s( m_FileName, sizeof(m_FileName), szFileName );
	printf( "%s file Compile\n", szFileName );

	m_Token = m_pScan->GetToken();
	if( ENDFILE == m_Token )
	{
		m_pScan->Clear();
		return NULL;
	}

	m_pRmiList = rmi_list();

	if( ENDFILE != m_Token )
	{
		SyntaxError( " code ends before file " );
		PrintToken( m_Token, m_pScan->GetTokenStringQ(0) );
		m_pScan->Clear();
		return NULL;
	}

	return m_pRmiList;
}


// rmi_list -> (rmi)*
sRmi* CProtocolParser::rmi_list()
{
	sRmi *p = rmi();
	sRmi *first = p;
	while (p && (p->next = rmi()))
	{
		p = p->next;
	}
	return first;
}

// rmi -> global id number '{' stmt_list '}'
sRmi* CProtocolParser::rmi()
{
	sRmi *p = NULL;
	
	if (PROTOCOL == m_Token)
	{
		Match(PROTOCOL);
		p = new sRmi;
		p->name = id();
		p->number = num();
		Match(LBRACE);
		p->protocol = stmt_list();
		Match(RBRACE);
	}

	return p;
}

// stmt_list -> (stmt)*
sProtocol* CProtocolParser::stmt_list()
{
	sProtocol *p = stmt();
	sProtocol *first = p;
	while (p && (p->next = stmt()))
	{
		p = p->next;
	}
	return first;
}

// stmt -> protocol semicolon
sProtocol* CProtocolParser::stmt()
{
	sProtocol *p = protocol();
	if (p)
		Match(SEMICOLON);
	return p;
}

// protocol -> id '(' arg_list ')'
sProtocol* CProtocolParser::protocol()
{
	sProtocol*p=NULL;
	if (ID == m_Token)
	{
		p = new sProtocol;
		p->name = id();
		Match(LPAREN);
		p->argList = arg_list();
		Match(RPAREN);
		p->next = NULL;
	}
	return p;
}


// arg_list -> [arg (',' arg)*]
sArg* CProtocolParser::arg_list()
{
	sArg *p = arg();
	if (!p)
		return NULL;

	sArg *first = p;
	while (COMMA == m_Token)
	{
		Match(COMMA);
		p->next = arg();
		p = p->next;
	}

	return first;
}

// arg -> type
sArg* CProtocolParser::arg()
{
	sArg *p = NULL;
	if (ID == m_Token)
	{
		p = new sArg;
		p->var = type();
		p->next = NULL;
	}
	return p;
}

// type -> type_sub (var)?
sTypeVar* CProtocolParser::type()
{
	sTypeVar *p=NULL;
	if (ID != m_Token)
		return NULL;

	Tokentype nextTok = m_pScan->GetTokenQ(1);
	p = new sTypeVar;
	p->type = type_sub();
	p->var = var();

	return p;
}

// type_sub -> id '<' type_sub '>'
//			| id::id
//			| id
std::string CProtocolParser::type_sub()
{
	std::string str = "";

	if (ID == m_Token)
	{
		Tokentype nextTok = m_pScan->GetTokenQ(1);
		if (LT == nextTok)
		{
			str += id();
			str += "<";
			Match(LT);
			str += type_sub();
			str += ">";
			Match(RT);
		}
		else if (SCOPE == nextTok)
		{
			str += id();
			str += "::";
			Match(SCOPE);
			str += type_sub();
		}
		else
		{
			str += id();
		}
	}

	return str;
}

// var -> '*' id (index)?
//	    | '&' id (index)?
//		| id (index)?
//	    | '*'
//		| '&'
std::string CProtocolParser::var()
{
	std::string str = "";
	Tokentype nextTok = m_pScan->GetTokenQ(1);

	if (TIMES == m_Token && ID == nextTok)
	{
		Match(TIMES);
		str += "*";
		str += id();
		str += index();
	}
	else if (REF == m_Token && ID == nextTok)
	{
		Match(REF);
		str += "&";
		str += id();
		str += index();
	}
	else if (ID == m_Token)
	{
		str += id();
		str += index();
	}
	else if (TIMES == m_Token)
	{
		Match(TIMES);
		str += "*";
	}
	else if (REF == m_Token)
	{
		Match(REF);
		str += "&";
	}

	return str;
}

std::string CProtocolParser::index()
{
	std::string str = "";
	if (LBRACKET == m_Token)
	{
		Match(LBRACKET);
		str += "[";
		str += number();
		str += "]";
		Match(RBRACKET);
	}
	return str;
}

std::string CProtocolParser::number()
{
	std::string str = "";
	str = m_pScan->GetTokenStringQ(0);
	Match(NUM);
	return str;
}

int CProtocolParser::num()
{
	int n = atoi(m_pScan->GetTokenStringQ(0));
	Match(NUM);
	return n;
}

std::string CProtocolParser::id()
{
	std::string str = m_pScan->GetTokenStringQ(0);
	Match( ID );
	return str;
}

BOOL CProtocolParser::Match( Tokentype t )
{
	if( m_Token == t )
	{
		m_Token = m_pScan->GetToken();
	}
	else
	{
		SyntaxError( "unexpected token -> " );
		PrintToken( m_Token, m_pScan->GetTokenStringQ(0) );
		printf( "\n" );
	}
	return TRUE;
}


void CProtocolParser::SyntaxError( char *szMsg, ... )
{
	m_bError = TRUE;
	char buf[ 256];
	va_list marker;
	va_start(marker, szMsg);
	vsprintf_s(buf, sizeof(buf), szMsg, marker);
	va_end(marker);
 	printf( ">>>" );
 	printf( "Syntax error at line %s %d: %s", m_FileName, m_pScan->GetLineNo(), buf );
}


void CProtocolParser::WritePIDLMacro(std::string PIDLFileName, sRmi *rmi)
{
// 	string fileName = PIDLFileName;
// 	fileName += "_procstub";

	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, PIDLFileName.c_str() );
	char *name = strtok_s(srcFileName, ".", NULL);

	std::string fileName = name;
	fileName += "_procstub.h";

//	OFSTRUCT of;
// 	HFILE hFile = OpenFile(fileName.c_str(), &of, OF_WRITE);
// 	if (hFile != EOF)

	FILE *fp;
	fopen_s(&fp, fileName.c_str(), "w" );
	if (!fp)
		return;

	fprintf( fp, "// Compiled by PIDLCompiler.exe\n" );
	fprintf( fp, "// CNetListener ���� ���ȴ�. ��Ŷ ó�� ���� ó���� �� �� �и��ϱ� ���� ���������.\n" );
	fprintf( fp, "// jjuiddong \n\n" );
	fprintf( fp, "#pragma once\n\n" );
	WriteRmi(fp, rmi);

	fclose(fp);
}

void CProtocolParser::WriteRmi(FILE *fp, sRmi *p)
{
	if (!p) return;
	
	fprintf( fp, "namespace %s {\n", p->name.c_str(), p->number );
	fprintf( fp, "\tclass ProcessStub {\n" );
	fprintf( fp, "\tpublic:\n" );

	WriteProtocol(fp, p, p->protocol);

	// BeforeRmiInvocation �������̽� �߰�
	fprintf( fp, "virtual void BeforeRmiInvocation(const Proud::BeforeRmiSummary& summary) {} \n" );

	fprintf( fp, "\t};\n" );
	fprintf( fp, "}\n\n" );

	WriteRmi(fp, p->next);
}

void CProtocolParser::WriteProtocol(FILE *fp, sRmi *rmi, sProtocol *p)
{
	if (!p) return;
	
//	printf( "\t\t%s( ", p->name.c_str() );

	// Before
	fprintf( fp, "#define DECRMI_%s_Before_%s bool Before_%s(", rmi->name.c_str(), p->name.c_str(), p->name.c_str() );
	WriteFirstArg(fp, p->argList);
	fprintf( fp, ")\n" );

	fprintf( fp, "#define DEFRMI_%s_Before_%s(DerivedClass) bool DerivedClass::Before_%s(", rmi->name.c_str(), 
		p->name.c_str(), p->name.c_str() );
	WriteFirstArg(fp, p->argList);
	fprintf( fp, ")\n" );

	fprintf( fp, "#define CALL_%s_Before_%s Before_%s(", rmi->name.c_str(), 
		p->name.c_str(), p->name.c_str() );
	WriteFirstArgVar(fp, p->argList);
	fprintf( fp, ")\n" );

	fprintf( fp, "virtual bool Before_%s(", p->name.c_str() );
	WriteFirstArg(fp, p->argList);
	fprintf( fp, ")\n{\n\nreturn false;\n}\n" );



	// After
	fprintf( fp, "#define DECRMI_%s_After_%s bool After_%s(", rmi->name.c_str(), p->name.c_str(), p->name.c_str() );
	WriteFirstArg(fp, p->argList);
	fprintf( fp, ")\n" );

	fprintf( fp, "#define DEFRMI_%s_After_%s(DerivedClass) bool DerivedClass::After_%s(", rmi->name.c_str(), 
		p->name.c_str(), p->name.c_str() );
	WriteFirstArg(fp, p->argList);
	fprintf( fp, ")\n" );

	fprintf( fp, "#define CALL_%s_After_%s After_%s(", rmi->name.c_str(), 
		p->name.c_str(), p->name.c_str() );
	WriteFirstArgVar(fp, p->argList);
	fprintf( fp, ")\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "virtual bool After_%s(", p->name.c_str() );
	WriteFirstArg(fp, p->argList);
	fprintf( fp, ")\n{\n\nreturn false;\n}\n" );

	fprintf( fp, "\n" );

	WriteProtocol(fp, rmi, p->next);
}

// ���ڰ� ó�� ��� 
// Proud::HostID remote,Proud::RmiContext &rmiContext �� �־���� �Ѵ�.
void CProtocolParser::WriteFirstArg(FILE *fp, sArg*p)
{
	fprintf( fp, "Proud::HostID remote, Proud::RmiContext &rmiContext" );
	WriteArg(fp, p, true);
}

// ������ Ÿ�԰� �̸��� ����Ѵ�.
void CProtocolParser::WriteArg(FILE *fp, sArg*p, bool isComma)
{
	if (!p) return;
	if (isComma)
		fprintf( fp, ", " );
	fprintf( fp, "const %s &%s", p->var->type.c_str(), p->var->var.c_str());
	WriteArg(fp, p->next, true);
}

// �����̸� ��� ó��
// Proud::HostID remote,Proud::RmiContext &rmiContext �� �־���� �Ѵ�.
void CProtocolParser::WriteFirstArgVar(FILE *fp, sArg*p)
{
	fprintf( fp, "remote, rmiContext" );
	WriteArgVar(fp, p, true);
}

// ���� �̸��� ����Ѵ�.
void CProtocolParser::WriteArgVar(FILE *fp, sArg*p, bool isComma)
{
	if (!p) return;
	if (isComma)
		fprintf( fp, ", " );
	fprintf( fp, "%s", p->var->var.c_str());
	WriteArgVar(fp, p->next, true);
}
