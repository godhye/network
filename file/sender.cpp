// echoclient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#pragma comment (lib , "ws2_32")
#include "헤더.h"
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
	if (nresult == SOCKET_ERROR)
	{
		puts("connect error");
		return -1;
	}
	tDATAEXP *data = new tDATAEXP;
	
	char file[BUFSIZ] = { 0, };
	char buf[BUFSIZ * 2] = { 0, };
	int len = 0;
	//send
	printf("client ======================= \n");
	size_t total = 0;
	 

	puts("file name :test.mp4 ");
	sprintf(file, "test.mp4");

	FILE *f = fopen(file, "rb");
	if (!f)
		return -1;
	int nresutl = 0;

	//파일사이즈 구하기
	//파일 끝으로 이동
	fseek(f, 0, SEEK_END);
	//현재 위치 
	size_t lsize = ftell(f);
	//다시 첨으로 이동 
	rewind(f);

	data->nfilesize = lsize;
	strcpy(data->szFilename, file);

	send(sServ, (char*)data, sizeof(tDATAEXP) , 0);

	while (1)
	{
		//fread
		nresutl = fread((void*)buf, 1, BUFSIZ * 2, f);
		total += nresutl;

		if (nresutl < BUFSIZ * 2)
		{
			int setlen = send(sServ, (char*)buf, nresutl, 0);
			printf("  client = total = %zd  , fread = %d setlen =%d \n", total, nresutl, setlen);
			break;
		}

		//send 할때 버퍼사이즈만큼 아니라 fread로 읽은 만큼 보내주기
		else
		{
			nresult = send(sServ, (char*)buf, BUFSIZ * 2, 0);
			//printf("  client = total = %zd  ,fread = %d \n", total, nresutl);
		}

	}
	printf("  client = total = %zd , lsize=%d \n", total, lsize);
	//파일사이즈만큼 보냈으면 , SHUT_WR
	shutdown(sServ, SD_SEND);
	delete data;
	//closesocket

	closesocket(sServ);
	//wsa clean
	WSACleanup();
	return 0;
}
