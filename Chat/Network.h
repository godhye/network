#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "���.h"
#include <errno.h>

class Network
{
public:
	Network();
	~Network();


	int Init(int nPort);
	int Work();

	int RecvMsg(int nindex); //���� ������ Ÿ�Կ� ���� �б� ó�� 
	int RecvData(int nIndex, int nDatasize, char* buf); //���� ������ Ÿ�Կ� ���� �б� ó�� 
	int SendMsg(int nSocket, int nChatRoom, int ServCode, int DataSize, char* szData, bool bDeleteData = true); //������ ���� 

	int Parsing(char * buf, int nrecvdata, char Sep, int &nServCode, int &nDataSize);
	virtual int OnConnect(int nSocket)=0;
	virtual int OnDisconnect(int nSocket) = 0;
	virtual int RecvHandler(int nSocket, int nServCode, int nDataSize, char * szData) = 0;
																												//servsock , addr
	SOCKET m_ServSock, m_CpyServ, m_ClntSock;
	SOCKADDR_IN m_Servaddr, m_Clntaddr;

	//FDSET  ���� ����
	fd_set m_Read, m_Cpyread;
	char m_recvbuf[10000] = { 0, };
	int m_nbufsize = 0;

	int m_nMaxRecv = 4;
	//�޼��� ť 

};

