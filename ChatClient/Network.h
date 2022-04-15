#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "헤더.h"
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
	
	int RecvMsg(int nindex); //받은 데이터 타입에 따른 분기 처리 
	int ClientRecvMsg(int nSocket); //받은 데이터 타입에 따른 분기 처리 

	int RecvData(int nIndex, int nDatasize, char* buf); //받은 데이터 타입에 따른 분기 처리 
	int ClientRecvData(int nIndex, int nDatasize, char* buf); //받은 데이터 타입에 따른 분기 처리 
	int SendMsg(int nSocket,  int ServCode, int DataSize, char* szData, bool bDeleteData = true); //데이터 전송 

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
	std::vector<tMSG> m_SendQ;
	std::mutex mu;

};

