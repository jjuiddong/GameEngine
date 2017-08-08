// ServerTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../../../Common/Common/common.h"
using namespace common;
#include "../../../../../Common/Network/network.h"

#pragma comment (lib, "winmm.lib")


using namespace std;
using namespace network;

int main()
{
	cTCPServer server;
	if (!server.Init(1010))
	{
		cout << "server binding error" << endl;
		return 0;
	}

	while (1)
	{
		sSockBuffer buffer;
		if (server.m_recvQueue.Front(buffer))
		{
			char buff[512];
			strncpy_s(buff, (char*)buffer.buffer, buffer.actualLen);
			buff[buffer.actualLen] = NULL;
			cout << buff << endl;

			server.m_recvQueue.Pop();
		}
		Sleep(100);
	}

    return 0;
}

