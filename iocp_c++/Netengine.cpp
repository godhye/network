#include "Netengine.h"


unsigned WINAPI ThreadMain(void*  pPara);

Netengine::Netengine()
{ 
	m_pServSock = new ServerSocket();

}

Netengine::~Netengine()
{

}

int Netengine::InitNet()
{


	
	int nclnt = 0;
	int nerror = 0;
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
		printf("WSAStartup() error!");


	HANDLE handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);


	if (!handle)
		return -1;

	_beginthreadex(NULL, 0, ThreadMain, (void*)handle, 0, NULL);

	SOCKADDR_IN servAdr;
	SOCKET hListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hListenSocket == INVALID_SOCKET) {
		nerror = WSAGetLastError();
	}
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(9999);

	//Bind()
	auto result = bind(hListenSocket, (SOCKADDR*)&servAdr, int(sizeof(servAdr)));
	 nerror = WSAGetLastError();

	//Listen()
	 int result1 = 1;
	 if (listen(hListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		 nerror = WSAGetLastError();
		 result1 = -1;
	}
	  
	while (result1>0)
	{
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		int addrLen = sizeof(clntAdr);

		hClntSock = accept(hListenSocket, (SOCKADDR*)&clntAdr, &addrLen);
		if (hClntSock == INVALID_SOCKET)
			continue;

		printf("NEW CLIENT \n");
		Sleep(199);
	}

	return 0;
}

int Socket::SetIpPort(char * pAddr, int nPort)
{
	 
	m_nPort  = (nPort);
	strcpy_s(m_pAddr, 100,pAddr);
	 
	return 0;
}

int Socket::Init()
{
	return 0;
}

int Socket::Delete()
{
	return 0;
}

int Socket::Send()
{
	return 0;
}

int Socket::Recv()
{
	return 0;
}

ServerSocket::ServerSocket()
{
}

ServerSocket::~ServerSocket()
{
}

int ServerSocket::Create()
{

	//Socket()
	
	return 0;
}

unsigned WINAPI ThreadMain(void*  pPara)
{
	HANDLE Handle = (HANDLE)pPara;

	int nByte = -1;
	SOCKET sock;
	LPPER_HANDLE_DATA handleinfo;
	LPPER_IO_DATA ioinfo;
	DWORD flags;

	while (1) {


		GetQueuedCompletionStatus(Handle, (LPDWORD)&nByte, (PULONG_PTR)&handleinfo, (LPOVERLAPPED*)&ioinfo, INFINITE);
		sock = handleinfo->hSock;

		if (ioinfo->rwMode == READ) {


		}
	}

	return 0;
}

//handle 생성 , thread 등록
int ServerSocket::CreateIOCP()
{



	return 0;
}
 
ClientSocket::ClientSocket()
{
}

ClientSocket::~ClientSocket()
{
}
