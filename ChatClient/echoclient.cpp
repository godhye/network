// echoclient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "헤더.h"
#pragma comment (lib , "ws2_32")

#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>


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

  
	printf("totallen %d \n", total);
	
	//wsa clean
	WSACleanup();
	return 0;
}
