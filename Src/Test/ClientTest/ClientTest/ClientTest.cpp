// ClientTest.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "../../../../../Common/Common/common.h"
using namespace common;
#include "../../../../../Common/Network/network.h"

#pragma comment (lib, "winmm.lib")


using namespace std;
using namespace network;


void PrintMenu()
{
	cout << "1. connection" << endl;
	cout << "2. send" << endl;
	cout << "3. close" << endl;
	cout << "4. quit" << endl;
}


int main()
{
	cTCPClient client;

	bool loop = true;
	while (loop)
	{
		PrintMenu();
		char c = getchar();
		switch (c)
		{
		case '1':
		{
			cout << "input ip port" << endl;
			cout << ">>";

			string ip;
			cin >> ip;
			string sport;
			getline(cin, sport);
			int port = atoi(sport.c_str());

			if (client.Init(ip, port))
			{
				cout << "Success Connection" << endl << endl;
			}
			else
			{
				cout << "Error Connection" << endl << endl;
			}
		}
		break;

		case '2':
		{
			cout << "send message" << endl;
			cout << ">>";

			string msg;
			cin >> msg;
			string line;
			getline(cin, line);
			line = msg + line;

			client.Send((BYTE*)line.c_str(), line.length());

		}
		break;

		case '3':
		{
			//loop = false;
			client.Close();
		}
		break;

		case '4':
		{
			loop = false;
		}
		break;
		}
	}

    return 0;
}

