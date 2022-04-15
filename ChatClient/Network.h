#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "���.h"
#include <errno.h>
#include <vector>
#include <mutex>
typedef struct sMSG
{
	unsigned int nSock;
	char* szbuf;
	size_t nsize;

}tMSG;

typedef std::vector<tMSG> MSGQ;

class Network
{
public:
	Network();
	~Network();


	int Init(int nPort);
	int Run();
	int RecvWork(); //server
	int SendWork(); //server

	int ClientRecvWork(); //client
	int ClientSendWork(); //client
	
	int RecvMsg(int nindex); //���� ������ Ÿ�Կ� ���� �б� ó�� 
	int ClientRecvMsg(int nSocket); //���� ������ Ÿ�Կ� ���� �б� ó�� 

	int RecvData(int nIndex, int nDatasize, char* buf); //���� ������ Ÿ�Կ� ���� �б� ó�� 
	int ClientRecvData(int nIndex, int nDatasize, char* buf); //���� ������ Ÿ�Կ� ���� �б� ó�� 
	int SendMsg(int nSocket,  int ServCode, int DataSize, char* szData, bool bDeleteData = true); //������ ���� 

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
	std::vector<tMSG> m_SendQ;
	std::mutex mu;

};

