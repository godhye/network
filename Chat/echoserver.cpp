// echoserver.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
	//winsock 동적라이브러리 연결 , 요구사항 충족하는지 확인 
	//winsock 2.2 버전 요청 
	int nresult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	return nresult;
}
int main()
{
	InitWinService();
	//conf(server ip , port  / data folder / keepalive 시간 ) 읽기
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
	//메모리해제 	
	delete sig;
	delete chatserver;

	return 0;
}
