#include "Netengine.h"


unsigned WINAPI ThreadMain(void*  pPara);


enum IOCPERROR
{

	IOCP_ERR1= -2,
	IOCP_ERR ,
	IOCP_OK,
};

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


	m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m_handle)
		return -1;
	 
	printf("m_handle! %d\n" , m_handle);
	_beginthreadex(NULL, 0, ThreadMain, m_handle, 0, NULL);

	return 0;
}

int Netengine::MakeClientSocket(int nServerType, char * addr, int port)
{
	ClientSocket *pSocket = new ClientSocket();
	if (pSocket) {

		pSocket->SetIpPort(addr, port);

		if (m_pCliSocks.find(nServerType) == m_pCliSocks.end())
		{
			m_pCliSocks[nServerType] = pSocket;
		}
		
		return IOCP_OK;
	}
	else
		return IOCP_ERR;
}

int Netengine::Run()
{
	//클라 접속
	//서버 접속 

	StartServer();

	return 0;
}

int Netengine::StartServer()
{
	int nError;
	//socket 생성
	SOCKET hListenSocket = m_pServSock->m_hSocket.hSock;
	hListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hListenSocket == INVALID_SOCKET) {
		nError = WSAGetLastError();
		return IOCP_ERR;
	}

	//bind

	SOCKADDR_IN servAdr;
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(m_pServSock->m_nPort);

	//Bind()
	if (bind(hListenSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {

		nError = WSAGetLastError();
		return IOCP_ERR;
	}
	 
	if (listen(hListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		nError = WSAGetLastError();
		return IOCP_ERR;
	}

	while (1)
	{
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		LPPER_HANDLE_DATA sHandle;
		LPPER_IO_DATA ioinfo;
		 int addrLen = sizeof(clntAdr);

		hClntSock = accept(hListenSocket, (SOCKADDR*)&clntAdr, &addrLen);
		if (hClntSock == INVALID_SOCKET)
			continue;

		printf("NEW CLIENT %d \n", hClntSock);

		sHandle = (LPPER_HANDLE_DATA) new PER_HANDLE_DATA;
		sHandle->hSock = hClntSock;
		memcpy(&sHandle->sockAdr, &clntAdr, sizeof(clntAdr));
		
		//만들어진 핸들에 소켓 등록 
		auto result = CreateIoCompletionPort((HANDLE)hClntSock, m_handle, (DWORD_PTR)hClntSock, 1);
		if (result == NULL) {
		
			printf("%d\n", GetLastError());
		}
		printf("handle %d\n", result);

		ioinfo = (LPPER_IO_DATA) new PER_IO_DATA;
		memset(&ioinfo->overlapped, 0, sizeof(OVERLAPPED));
		memset(ioinfo->buffer, 0, BUF_SIZE);
		ioinfo->wsaBuf.buf = ioinfo->buffer;
		ioinfo->wsaBuf.len = BUF_SIZE;
		ioinfo->rwMode = READ;

		DWORD recvbytes= 0;
		DWORD flags =0;

		WSARecv(sHandle->hSock, &(ioinfo->wsaBuf), 1, &recvbytes, &flags, &(ioinfo->overlapped), NULL);
		 
		Sleep(199);
	}
	//listen
	
	//accept

	return 0;
}

int Socket::SetIpPort(char * pAddr, int nPort)
{
	 
	m_nPort  = (nPort);
	strcpy_s(m_pAddr, strlen(pAddr) +1 ,pAddr);
	m_uiaddr = inet_addr(pAddr);

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

	printf("%s m_handle! %d\n",__FUNCTION__,  Handle);
	DWORD nByte = -1;
	SOCKET sock;
	int iocpkey;
	LPPER_HANDLE_DATA handleinfo =NULL;
	LPPER_IO_DATA ioinfo =NULL;

	DWORD flags=0;

	while (1) {


		GetQueuedCompletionStatus(Handle,  &nByte, (PULONG_PTR)&iocpkey, (LPOVERLAPPED*)&ioinfo, INFINITE);
		
		 sock = iocpkey;

		
		if (ioinfo->rwMode == READ) {
			if (nByte == 0) {
				//연결 종료 
				printf(" Quit %d \n", sock);
				delete ioinfo;
				delete handleinfo;
			}

			printf("READ %s\n", ioinfo->buffer);

			memset(&(ioinfo->overlapped), 0, sizeof(OVERLAPPED));
			ioinfo->wsaBuf.len = nByte;
			ioinfo->rwMode = WRITE;

			WSASend(sock, &(ioinfo->wsaBuf), 1, NULL, 0, &(ioinfo->overlapped), NULL);

			ioinfo = (LPPER_IO_DATA)new PER_IO_DATA;
			
			memset(&ioinfo->overlapped, 0, sizeof(OVERLAPPED));
			memset(&ioinfo->wsaBuf, 0, BUF_SIZE);
			ioinfo->wsaBuf.buf = ioinfo->buffer;
			ioinfo->wsaBuf.len = BUF_SIZE;
			ioinfo->rwMode = READ;
			WSARecv(sock, &(ioinfo->wsaBuf), 1, NULL, &flags, &(ioinfo->overlapped), NULL);
		}
		else {
			printf(" message sent %d \n", sock);
			delete ioinfo;
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
