// echoclient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#pragma comment (lib , "ws2_32")

#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>
#define USE_PORT 4444
#define BUFSIZE 300
#define ADDR "127.0.0.1"
int main()
{

	//wsa startup
	WSADATA wsaData;
	SOCKADDR_IN sServaddr;
	SOCKET sServ, sClnt;

	//winsock 동적라이브러리 연결 , 요구사항 충족하는지 확인 
	//winsock 2.2 버전 요청 
	int nresult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//socket
	sServ = socket(AF_INET, SOCK_STREAM, 0);
	if (sServ == INVALID_SOCKET)
	{
		puts("hServ error");
		return -1;
	}

	//구조체 값 한꺼번에 초기화
	memset(&sServaddr, 0, sizeof(SOCKADDR));
	//AF_INET = ipv4  주소체계 프로토콜
	sServaddr.sin_family = AF_INET;
	sServaddr.sin_addr.s_addr = inet_addr(ADDR);
	sServaddr.sin_port = USE_PORT;
	 
	//connect
	nresult = connect(sServ, (SOCKADDR*)&sServaddr, sizeof(sServaddr));
	if (nresult== SOCKET_ERROR)
	{
		puts("connect error");
		return -1;
	}

	TCHAR buf[BUFSIZ] = { 0, };
	int len = 0;
	//send
	printf("client ======================= \n");
	int total = 0;
	while (1)
	{
		//q 이면 종료
		fputs("input message (q to quit)\n", stdout);
		fgets(buf ,BUFSIZ,stdin);
		if (strcmp(buf, "q\n") == 0)
			break;

		printf("strlen = %d\n\n", strlen(buf));

		len = send(sServ, buf, strlen(buf), 0);
		//recv
		len = recv(sServ, buf, BUFSIZ-1, 0);
		buf[len] = 0;

		total += len;
 		if(len == -1)
			fputs("len -1 \n", stdout);
		//출력
		printf("recv : %s %d \n", buf, len);

	}
	//closesocket
	printf("totallen %d \n", total);
	closesocket(sServ);
	//wsa clean
	WSACleanup();
    return 0;
}

