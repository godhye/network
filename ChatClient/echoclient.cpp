// echoclient.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "���.h"
#pragma comment (lib , "ws2_32")

#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>


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

  
	printf("totallen %d \n", total);
	
	//wsa clean
	WSACleanup();
	return 0;
}
