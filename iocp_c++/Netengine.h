#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>
#include <vector>
#include <map>

//#include <thread>
#include <WS2tcpip.h>
#define BUF_SIZE 1024
#define READ	3
#define	WRITE	5

using namespace std;
 
// socket info
typedef struct
{
	SOCKET hSock;
	SOCKADDR_IN sockAdr;
	 
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// buffer info
typedef struct
{
	OVERLAPPED overlappedrecv; //recv
	WSABUF wsaRecvBuf;
	OVERLAPPED overlappedsend; //send
	WSABUF wsaSendBuf;

	char Recvbuffer[BUF_SIZE];
	char Sendbuffer[BUF_SIZE];
	int rwMode;    // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;


DWORD WINAPI EchoThreadMain(HANDLE CompletionPortIO);
void ErrorHandling(char *message);
 
class Socket {

public:
	PER_HANDLE_DATA m_hSocket;
	LPPER_IO_DATA  m_ioBuffer;
	int m_nSess;
	const char* m_szServerName = "";
	char m_pAddr[100] ={0,};
	unsigned int m_uiaddr = 0;
	int m_nPort = -1 ;
	 
public:
	int SetIpPort(char* pAddr, int nPort);
	 int Init();
	 int Delete();
	 int Send();
	 int Recv();
};


class ServerSocket : public Socket{

public:
	ServerSocket();
	~ServerSocket();
	//BIND , LISTEN , ACCEPT
	int Create();
	int CreateIOCP();

	HANDLE m_Handle;
};


class ClientSocket : public Socket {
	


public:
	ClientSocket();
	~ClientSocket();
	//접속할 아이피 . 포트 
	
	//Connect
	int Delete(int nSess);


};

class Netengine
{
	
public:
	Netengine();
	~Netengine();

	int InitNet();
	int MakeClientSocket(int nServerType,  char *addr, int port);
	int Run();

	int StartServer();

	//적당한 유저유니크 
	int GetIndex();

	HANDLE m_handle = NULL;

	WSADATA	m_wsaData;
	ServerSocket *m_pServSock; 
	std::map<int , ClientSocket *> m_pCliSocks;

	int m_iTreeType = 0;
	
};

