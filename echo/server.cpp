// echoserver.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
 
#pragma comment (lib , "ws2_32")


#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>
#define USE_PORT 4444
#define BUFSIZE 300

int main()
{
	
	//wsa 초기화
	WSADATA wsaData;

	//winsock 동적라이브러리 연결 , 요구사항 충족하는지 확인 
	//winsock 2.2 버전 요청 
	int nresult = WSAStartup(MAKEWORD(2,2),&wsaData );
	if (nresult != 0)
	{
		fputs("WSAStartup error", stdin);
		return -1;
	}

	SOCKET hServ, hCnt;
	SOCKADDR_IN sServaddr, sClntaddr;
	
	//socket(), 소켓할당 
	//PF_INET = ipv4 프로토콜
	//sock_stream = tcp
	hServ= socket(PF_INET, SOCK_STREAM, 0);
	if (hServ == INVALID_SOCKET)
	{
		fputs("WSAStartup error", stdin);
		puts("hServ error");
		return -1;
	}

	//구조체 값 한꺼번에 초기화
	memset(&sServaddr, 0, sizeof(SOCKADDR));
	//AF_INET = ipv4  주소체계 프로토콜
	sServaddr.sin_family = AF_INET;
	sServaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sServaddr.sin_port = USE_PORT;
	printf("server ======================= \n");
	//bind
	nresult = bind(hServ, (SOCKADDR*)&sServaddr, sizeof(sServaddr));
	if (nresult != 0)
	{
		fputs("WSAStartup error", stdin);
		puts("bind error");
		return -1;
	}

	//연결대기 backlog , 연결대기큐갯수 , 모두 찬 상태로 새로운 연결이면 ECONNREPUSED
	nresult = listen(hServ, 5);
	if (nresult != 0)
	{
		fputs("WSAStartup error", stdin);
		puts("listen error");
		return -1;
	}
	fputs("listen  ", stdin);
	int clntsize = sizeof(sClntaddr);
	for (int i = 0; i < 5; i++)
	{

	
		//accept
		hCnt = accept(hServ, (SOCKADDR*)&sClntaddr, &clntsize);
		if (hCnt == -1)
		{
			fputs("WSAStartup error", stdin);
			puts("hCnt error");
			return -1;
		}
		printf("connected \n");
		//read/write
		int strlen = 0;
		TCHAR buf[BUFSIZ] = { 0, };

		while ((strlen = recv(hCnt, buf, BUFSIZ, 0)) != 0)
		{
			if (strlen == -1)
			{
				printf("CONNET ERROR  \n");
				break;
			}
			int sendlen = send(hCnt, buf, strlen, 0);
			printf("sendlen %d \n", sendlen);
		}
		//close
		printf("close \n", buf);
		nresult = closesocket(hCnt);

	}
	
	//소켓정리 
	closesocket(hServ);
	//wsa 정리
	WSACleanup();
    return 0;
}

