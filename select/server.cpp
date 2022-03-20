// echoserver.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#pragma comment (lib , "ws2_32")

#include "헤더.h"
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#define USE_PORT 4444
#define BUFSIZE 300

int main()
{

	//wsa 초기화
	WSADATA wsaData;

	//winsock 동적라이브러리 연결 , 요구사항 충족하는지 확인 
	//winsock 2.2 버전 요청 
	int nresult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nresult != 0)
	{
		fputs("WSAStartup error", stdin);
		return -1;
	}
	
	SOCKET servsock,cpyserv, clntsock;
	SOCKADDR_IN servaddr, clntaddr;

	servsock = socket(PF_INET, SOCK_STREAM, 0);
	cpyserv = servsock;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = USE_PORT;

	int sockopt = 1;
	setsockopt(servsock, SOL_SOCKET, SO_REUSEADDR, (char*)&sockopt, sizeof(sockopt));
	bind(servsock, (sockaddr*)&servaddr, sizeof(servaddr));
	
	listen(servsock, 5);

	
	//파일디스크립터(소켓 , [수신/전송/예외] 설정

	//cpyread 하는 이유 원본 read 변화있는 디스크립터 처리후 변화없는 디스크립터들 0으로 초기화 
 
	fd_set read, cpyread;
	FD_ZERO(&read);

	//read에 servsock 소켓 등록 
	FD_SET(servsock, &read);
	int hserv = servsock;
	TIMEVAL timeout;

	while (1)
	{
 
		//왜 복사 필요한가?
		cpyread = read;
		timeout.tv_sec = 5;
		timeout.tv_usec = 100000;
		int fdNum;

		//cpyread 반드시 써야함 
		if ((fdNum = select(0, &cpyread, 0, 0, &timeout)) == SOCKET_ERROR)
		{
			break;
		}
		//호출 결과 확인 
		//변화된 소켓 없을때 
		if (fdNum == 0)
		{
			continue;
		}

		/*if (servsock == 0)
		{
			printf("servsock err\n");
			break;
		}*/
		for (int i = 0; i < read.fd_count; i++)
		{
			if (FD_ISSET(read.fd_array[i], &cpyread))
			{
				//변화있는 소켓이 서버소켓 == 새로운 연결 요청 
				if (read.fd_array[i] == servsock)
				{
					printf("새로운 연결 요청 server  clnt = %d  servsock = %d \n", read.fd_array[i], servsock);
					int adSz = sizeof(clntaddr);
					clntsock = accept(servsock, (SOCKADDR*)&clntsock, &adSz);
					//새로 연결된 클라이언트소켓 등록
					FD_SET(clntsock, &read);
					printf("new user %d\n", clntsock);
				}

				else //read message
				{


					char buf[BUFSIZ];
					int recvlen = recv(read.fd_array[i], buf, BUFSIZ - 1, 0);

					

					if (recvlen < 0)
					{
						printf("recv 오류 clnt = %d servsock = %d\n", read.fd_array[i], servsock);
						FD_CLR(read.fd_array[i], &read);
						closesocket(cpyread.fd_array[i]);
					}

					else if (recvlen == 0)
					{

						
						FD_CLR(read.fd_array[i], &read);
						closesocket(cpyread.fd_array[i]);
						printf("연결종료  clnt = %d servsock =%d \n", read.fd_array[i], servsock);
					}

					else
					{
						buf[recvlen] = '\0';
						printf("recv msg = %s \n", buf);
						send(read.fd_array[i], buf, recvlen, 0);
						//send(read.fd_array[i], buf, BUFSIZ, 0);
					}



				}
			}
		}
	}


	// SELECT 호출


 
	closesocket(clntsock);
	closesocket(servsock);
	//wsa 정리
	WSACleanup();
	return 0;
}
