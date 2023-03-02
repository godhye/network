 
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
