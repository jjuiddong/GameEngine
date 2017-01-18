
#include "stdafx.h"
#include "Launcher.h"
#include "CoreClient.h"

using namespace network;

//------------------------------------------------------------------------
// 서버 시작
//------------------------------------------------------------------------
bool launcher::LaunchServer(ServerBasicPtr pSvr, int port)
{
	if (!pSvr)
		return false;

	// 윈속을 시작하고 버전을 확인합니다
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if(wsaData.wVersion != wVersionRequested)
	{
		clog::Error( clog::ERROR_CRITICAL,  "윈속 버전이 틀렸습니다\n" );
		return false;
	}

	// socket(주소계열, 소켓 형식, 프로토콜)
	SOCKET svrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(svrSocket == INVALID_SOCKET)
	{
		clog::Error( clog::ERROR_CRITICAL, "socket() error\n" );
		return false;
	}

	// 주소 구조체를 채웁니다.
	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;    // 윈속이 제공하게 둡니다.
	saServer.sin_port = htons(port);		// 명령줄에서 받은 포트를 넣습니다.

	// 소켓과 listensocket 을 bind(묶습) 합니다.
	// bind(소켓, 서버 주소, 주소 구조체의 길이
	nRet = bind(svrSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if(nRet == SOCKET_ERROR)
	{
		clog::Error( clog::ERROR_CRITICAL,  "bind() error port: %d\n", port );
		closesocket(svrSocket);
		return false;
	}

	int nLen;
	nLen = sizeof(SOCKADDR);
	char szBuf[256];

	nRet = gethostname( szBuf, sizeof(szBuf) );
	if (nRet == SOCKET_ERROR)
	{
		clog::Error( clog::ERROR_CRITICAL, "gethostname() error\n" );
		closesocket(svrSocket);
		return false;
	}

	// listen(오는 소켓, 요청 수용 가능한 용량)
	nRet = listen(svrSocket, SOMAXCONN);

	if (nRet == SOCKET_ERROR)
	{
		clog::Error( clog::ERROR_CRITICAL, "listen() error\n" );
		closesocket(svrSocket);
		pSvr->OnDisconnect();
		return false;
	}

	pSvr->SetSocket(svrSocket);
	pSvr->SetIp("127.0.0.1");
	pSvr->SetPort(port);
	pSvr->OnListen();
	return true;
}


//------------------------------------------------------------------------
// 클라이언트 시작
//------------------------------------------------------------------------
bool launcher::LaunchClient(ClientPtr pClient, const std::string &ip, int port)
{
	if (!pClient)
		return false;

	if (!LaunchCoreClient(pClient->GetConnectSvrClient(), ip, port))
		return false;

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	launcher::LaunchCoreClient(CoreClientPtr pClient, const std::string &ip, int port)
{
	if (!pClient)
		return false;

	pClient->SetConnect(false);

	// 윈속 버전을 확인 합니다.
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		clog::Error( clog::ERROR_CRITICAL, "윈속 버전이 틀렸습니다\n" );
		return false;
	}

	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(ip.c_str());
	if(lpHostEntry == NULL)
	{
		clog::Error( clog::ERROR_CRITICAL, "gethostbyname() error\n" );
		return false;
	}

	// TCP/IP 스트림 소켓을 생성합니다.
	// socket(주소 계열, 소켓 형태, 프로토콜
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		clog::Error( clog::ERROR_CRITICAL, "socket() error\n" );
		return false;
	}

	// 주소 구조체를 채웁니다.
	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list); // 서버 주소
	saServer.sin_port = htons(port);

	// 서버로 접속합니다
	// connect(소켓, 서버 주소, 서버 주소의 길이
	nRet = connect(clientSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if(nRet == SOCKET_ERROR)
	{
		clog::Error( clog::ERROR_CRITICAL, "connect() error ip=%s, port=%d\n", ip.c_str(), port );
		closesocket(clientSocket);
		pClient->OnDisconnect();
		return false;
	}

	pClient->SetIp(ip);
	pClient->SetPort(port);
	pClient->SetSocket(clientSocket);
	pClient->SetState(SESSIONSTATE_LOGIN);
	pClient->OnConnect();

	return true;
}
