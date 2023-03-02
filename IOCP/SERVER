//#include <stdio.h>
//#include <stdlib.h>
//#include <process.h>
//#include <stdio.h>
//#include <WinSock2.h>
//#include <winsock.h>
//
//
//
//
//#define BUF_SIZE 1000
//#define READ 3
//#define WRITE 5
//typedef struct
//{
//	SOCKET hClntSocket;
//	SOCKADDR_IN clntAdr;
//}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;
//
//
//typedef struct
//{
//	OVERLAPPED overlapped;
//	WSABUF wsaBuf;
//	char buf[BUF_SIZE];
//	int rwMode;
//
//}PER_IO_DATA, *LPPER_IO_DATA;
//
//DWORD WINAPI EchoThreadMain(LPVOID CPIO);
//
//
//
//DWORD WINAPI EchoThreadMain(LPVOID CPIO)
//{
//	HANDLE hCP = (HANDLE)CPIO;
//	SOCKET sock;
//	DWORD byteTrans;
//	LPPER_HANDLE_DATA handleinfo;
//	LPPER_IO_DATA ioinfo;
//	DWORD flags = 0;
//
//
//	while (1)
//	{
//		GetQueuedCompletionStatus(hCP, &byteTrans, (LPDWORD)&handleinfo, (LPOVERLAPPED)&ioinfo, INFINITE);
//		if (handleinfo->hClntSocket) {
//		
//			sock = handleinfo->hClntSocket;
//
//			if (ioinfo->rwMode == READ)
//			{
//				if (byteTrans == 0)
//				{
//					closesocket(sock);
//					free(handleinfo);
//					free(ioinfo);
//					continue;
//				}
//
//				memset(&(ioinfo->overlapped), 0, sizeof(OVERLAPPED));
//				ioinfo->wsaBuf.len = byteTrans;
//				ioinfo->rwMode = WRITE;
//				WSASend(sock, &(ioinfo->wsaBuf), 1, NULL, 0, &(ioinfo->overlapped), NULL);
//
//
//				ioinfo = (LPPER_IO_DATA)malloc(sizeof(LPPER_IO_DATA));
//				memset(&(ioinfo->overlapped), 0, sizeof(OVERLAPPED));
//				ioinfo->wsaBuf.len = BUF_SIZE;
//				ioinfo->wsaBuf.buf = ioinfo->buf;
//				ioinfo->rwMode = READ;
//				WSARecv(sock, &(ioinfo->wsaBuf), 1, NULL, 0, &ioinfo->overlapped, NULL);
//			}
//			else
//			{
//				//message sent
//				free(ioinfo);
//			}
//		}
//		
//	}
//	return 0;
//}
//
//
//void ErrorHandling(const char* message)
//{
//	printf("%s\n", message);
//}
//
//int main()
//{
//
//	WSADATA wsaData;
//	HANDLE hCP;
//	SYSTEM_INFO sysinfo;
//	LPPER_HANDLE_DATA handleinfo;
//	LPPER_IO_DATA ioinfo;
//
//	SOCKET  hServSock;
//	SOCKADDR_IN   ServAdr;
//
//	int recvBytes, i, flags = 0;
//
//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//	{
//		ErrorHandling("startup error");
//	}
//
//
//	hCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//	GetSystemInfo(&sysinfo);
//
//	for (int i = 0; i < 1; i++)
//	//for (int i = 0; i < sysinfo.dwNumberOfProcessors; i++)
//	{
//		_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hCP, 0, NULL);
//	}
//	//Overlapped용 소켓 생성
//	hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//	//bind
//	bind(hServSock, (SOCKADDR*)&ServAdr, sizeof(ServAdr));
//	memset(&ServAdr, 0, sizeof(ServAdr));
//	ServAdr.sin_family = AF_INET;
//	ServAdr.sin_addr.s_addr = htonl(INADDR_ANY);
//	ServAdr.sin_port = 4444;
//
//	//listen
//	listen(hServSock, 5);
//
//	while (1)
//	{
//		SOCKET hclntSock;
//		SOCKADDR_IN clntAdr;
//		int addrLen = sizeof(clntAdr);
//		hclntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
//		handleinfo = (LPPER_HANDLE_DATA)malloc(sizeof(LPPER_HANDLE_DATA));
//
//		handleinfo->hClntSocket = hclntSock;
//		memcpy(&(handleinfo->clntAdr), &clntAdr, addrLen);
//
//		//소켓을 hCP에 등록 
//		CreateIoCompletionPort((HANDLE)hclntSock, hCP, (DWORD)handleinfo, 0);
//
//		ioinfo = (LPPER_IO_DATA)malloc(sizeof(LPPER_IO_DATA));
//		memset(ioinfo, 0, sizeof(LPPER_IO_DATA));
//		ioinfo->wsaBuf.len = BUF_SIZE;
//		ioinfo->wsaBuf.buf = ioinfo->buf;
//		ioinfo->rwMode = READ;
//		WSARecv(handleinfo->hClntSocket, &ioinfo->wsaBuf, 1, &recvBytes, &flags, &ioinfo->overlapped, NULL);
//
//	}
//
//	return 0;
//}
// 

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#define BUF_SIZE 1000
#define READ	3
#define	WRITE	5


// socket info
typedef struct    
{
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
	int nSess;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// buffer info
typedef struct    
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode;    // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI EchoThreadMain(HANDLE CompletionPortIO);
void ErrorHandling(char *message);

//void *

int main(int argc, char* argv[])
{
	WSADATA	wsaData;
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;
	LPPER_HANDLE_DATA handleInfo;
	HANDLE hComPort;
	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	int recvBytes, i, flags = 0;


	int nclnt = 0;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");


	// CREATE CP HANDLE 
	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	printf("========= SERVER START =========== \n" , hComPort);
	GetSystemInfo(&sysInfo);

	//MAKE THREAD PROCESSING SOCKET IO 
	for (i = 0; i < sysInfo.dwNumberOfProcessors; i++)
		_beginthreadex(NULL, 0, EchoThreadMain, hComPort, 0, NULL);
	 
	//MAKE SOCKET OVERLAPPED!
	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(4444);

	bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	listen(hServSock, 5);

	while (1)
	{
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		int addrLen = sizeof(clntAdr);

		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		printf("NEW CLIENT \n");
		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handleInfo->hClntSock = hClntSock;
		handleInfo->nSess = nclnt++;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		//만들어진 핸들에 소켓 등록 
		CreateIoCompletionPort((HANDLE)hClntSock, hComPort,handleInfo, 0);

		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		memset(ioInfo->buffer, 0, BUF_SIZE);
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->rwMode = READ;

		//사용할 IOINFO를 전달하기위해 호출  , recvBytes 수신작업이 즉시 완료되는 경우 이 호출에서 받은 데이터 수에 대한 포인터 
		//ioInfo->overlapped) 이 NULL이 아닌 경우에만 recvBytes가 null일수 있다. 
		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf),1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
			
	}
	return 0;
}

DWORD WINAPI EchoThreadMain(HANDLE pComPort)
{
	 
	HANDLE hComPort = (HANDLE)pComPort;
	 
	SOCKET sock;
	DWORD bytesTrans;
	//소켓 정보 가져오기 위해 존재 
	LPPER_HANDLE_DATA handleInfo;
	//IO 정보 가져오기 
	LPPER_IO_DATA ioInfo;
	DWORD flags = 0;
	int recvBytes =0;
	while (1)
	{
		//완료된 IO확인 
		GetQueuedCompletionStatus(pComPort, &bytesTrans,
			 &handleInfo,  &ioInfo, INFINITE);
		sock = handleInfo->hClntSock;

		if (ioInfo->rwMode == READ)
		{
			if (bytesTrans == 0)    // EOF 전송 시
			{
				closesocket(sock);
				free(handleInfo); free(ioInfo);
				continue;
			}
			
			printf("[%d] message received! %s \n", handleInfo->nSess ,ioInfo->buffer );
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = bytesTrans;
			ioInfo->rwMode = WRITE;
			WSASend(sock, &(ioInfo->wsaBuf),
				1, NULL, 0, &(ioInfo->overlapped), NULL);

			ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->wsaBuf.buf = ioInfo->buffer;
			ioInfo->rwMode = READ;
			WSARecv(sock, &(ioInfo->wsaBuf),1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else
		{
			printf("message sent!\n");
			free(ioInfo);
		}
	}
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
