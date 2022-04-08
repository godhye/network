// echoserver.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#pragma comment (lib , "ws2_32")


#include "ChatServer.h"
#define USE_PORT 4444
#define BUFSIZE 300

int InitWinService()
{
	//wsa startup
	WSADATA wsaData;
	//winsock �������̺귯�� ���� , �䱸���� �����ϴ��� Ȯ�� 
	//winsock 2.2 ���� ��û 
	int nresult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	return nresult;
}
int main()
{
	InitWinService();
	//conf(server ip , port  / data folder / keepalive �ð� ) �б�
	int nPort = USE_PORT;

	//ChatServer create 
	ChatServer *chatserver = new ChatServer();
	if (chatserver->Init(nPort) < 0)
	{
		printf("Init ERROR ");
		return 0;
	}
	
	auto sig = CreateEvent(NULL, true, false, NULL);

	
	chatserver->Work();
	//WaitForSingleObject(sig, INFINITE);
	//�޸����� 	
	delete sig;
	delete chatserver;

	return 0;
}
