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
	_beginthreadex(NULL, 0, ThreadMain, this, 0, NULL);

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

	//IOCP 서버 접속 
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

	int clnt = 0;
	while (1)
	{
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr; 
		LPPER_IO_DATA ioinfo;
		 int addrLen = sizeof(clntAdr);

		hClntSock = accept(hListenSocket, (SOCKADDR*)&clntAdr, &addrLen);
		if (hClntSock == INVALID_SOCKET)
			continue;

		
		//printf("NEW CLIENT %d \n", hClntSock);
		
		
		ioinfo = (LPPER_IO_DATA) new PER_IO_DATA;
		memset(&ioinfo->overlappedrecv, 0, sizeof(OVERLAPPED));
		memset(ioinfo->Recvbuffer, 0, BUF_SIZE);
		ioinfo->wsaRecvBuf.buf = ioinfo->Recvbuffer;
		ioinfo->wsaRecvBuf.len = BUF_SIZE;

		memset(&ioinfo->overlappedsend, 0, sizeof(OVERLAPPED));
		memset(ioinfo->Sendbuffer, 0, BUF_SIZE);
		ioinfo->wsaSendBuf.buf = ioinfo->Sendbuffer;
		ioinfo->wsaSendBuf.len = BUF_SIZE;

		ioinfo->rwMode = READ;
		

		int isess = GetIndex();
		printf("new client %d\n", isess);
		if (isess >=0) {
			m_pCliSocks[isess] = new ClientSocket;

			m_pCliSocks[isess]->m_hSocket.hSock = hClntSock;
			m_pCliSocks[isess]->m_ioBuffer = ioinfo;
		}

		//만들어진 핸들에 소켓 등록 
		auto result = CreateIoCompletionPort((HANDLE)hClntSock, m_handle, (DWORD_PTR)isess, 1);
		if (result == NULL) {

			printf("%ERR d\n", GetLastError());
		}

		
		DWORD recvbytes= 0;
		DWORD flags =0;

		//overlappedrecv 객체 전달용
		WSARecv(hClntSock, &(ioinfo->wsaRecvBuf), 1, &recvbytes, &flags, &(ioinfo->overlappedrecv), NULL);
		 
		Sleep(199);
	}
	//listen
	
	//accept

	return 0;
}

int Netengine::GetIndex()
{
	for (int index = 0; index < 100; index++)
	{
		if (m_pCliSocks.find(index) == m_pCliSocks.end())
		{
			return index;
		}
	}
	//메모리 부족 
	return -1;
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
	Netengine *pEngine = (Netengine*)pPara;

	DWORD nByte = -1;
	SOCKET sock;
	int iocpkey; ;
	OVERLAPPED *over;

	DWORD flags=0;

	while (1) {

		GetQueuedCompletionStatus(pEngine->m_handle, &nByte, (PULONG_PTR)&iocpkey, (LPOVERLAPPED*)&over, INFINITE);

		if (iocpkey >= 0)
			sock = pEngine->m_pCliSocks[iocpkey]->m_hSocket.hSock;

		OVERLAPPED *RecvOver = &pEngine->m_pCliSocks[iocpkey]->m_ioBuffer->overlappedrecv;
		OVERLAPPED *SendOver = &pEngine->m_pCliSocks[iocpkey]->m_ioBuffer->overlappedsend;
		LPPER_IO_DATA  ioinfo = pEngine->m_pCliSocks[iocpkey]->m_ioBuffer;

		if (ioinfo == NULL || sock == INVALID_SOCKET) {
			continue;
		}

		if (over  == RecvOver) {
			if (nByte == 0) {
				//연결 종료 
				printf(" Quit %d \n", iocpkey);
				//메모리 해제 해야함
				auto it = pEngine->m_pCliSocks.find(iocpkey);
				if (it != pEngine->m_pCliSocks.end())
				{
					delete pEngine->m_pCliSocks[iocpkey]->m_ioBuffer;
					delete pEngine->m_pCliSocks[iocpkey];
					pEngine->m_pCliSocks.erase(it);
				}
				continue;
			}

			//printf("overlappedrecv %p  %p \n", &ioinfo->overlappedrecv, &ioinfo->overlappedsend);

			//echo send 
			memset(&(RecvOver), 0, sizeof(OVERLAPPED));
			 ioinfo->wsaSendBuf.buf = ioinfo->Recvbuffer;
			ioinfo->wsaSendBuf.len = nByte;
			ioinfo->rwMode = WRITE;
			 
			WSASend(sock, &(ioinfo->wsaSendBuf), 1, NULL, 0, &(ioinfo->overlappedsend), NULL);

			 
			//recv iocp 등록
			memset(&ioinfo->overlappedrecv, 0, sizeof(OVERLAPPED));
			memset(&ioinfo->wsaRecvBuf, 0, BUF_SIZE);
			ioinfo->wsaRecvBuf.buf = ioinfo->Recvbuffer;
			ioinfo->wsaRecvBuf.len = BUF_SIZE;
			ioinfo->rwMode = READ;
			WSARecv(sock, &(ioinfo->wsaRecvBuf), 1, NULL, &flags, &(ioinfo->overlappedrecv), NULL);
		}
		else {
			printf(" message sent %d => %d \n", iocpkey, nByte);
			//delete ioinfo;
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
