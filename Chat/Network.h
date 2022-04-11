#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "헤더.h"
#include <errno.h>

class Network
{
public:
	Network();
	~Network();


	int Init(int nPort);
	int Work();

	int RecvMsg(int nindex); //받은 데이터 타입에 따른 분기 처리 
	int RecvData(int nIndex, int nDatasize, char* buf); //받은 데이터 타입에 따른 분기 처리 
	int SendMsg(int nSocket, int nChatRoom, int ServCode, int DataSize, char* szData, bool bDeleteData = true); //데이터 전송 

	int Parsing(char * buf, int nrecvdata, char Sep, int &nServCode, int &nDataSize);
	virtual int OnConnect(int nSocket)=0;
	virtual int OnDisconnect(int nSocket) = 0;
	virtual int RecvHandler(int nSocket, int nServCode, int nDataSize, char * szData) = 0;
																												//servsock , addr
	SOCKET m_ServSock, m_CpyServ, m_ClntSock;
	SOCKADDR_IN m_Servaddr, m_Clntaddr;

	//FDSET  소켓 보관
	fd_set m_Read, m_Cpyread;
	char m_recvbuf[10000] = { 0, };
	int m_nbufsize = 0;

	int m_nMaxRecv = 4;
	//메세지 큐 

};

