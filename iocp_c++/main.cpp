#include "Netengine.h"
//void *

int main(int argc, char* argv[])
{


	Netengine *p = new Netengine();
	
	//p.m_pServSock->SetIpPort((char*)"127.0.0.1", 8888);


	p->InitNet();

	//p.m_pServSock->CreateIOCP();
	//p.m_pServSock->Create();

	return 0;
}

//
//unsigned WINAPI EchoThreadMain(void* pComPort)
//{
//
//	HANDLE hComPort = (HANDLE)pComPort;
//
//	SOCKET sock;
//	DWORD bytesTrans;
//	//소켓 정보 가져오기 위해 존재 
//	LPPER_HANDLE_DATA handleInfo;
//	//IO 정보 가져오기 
//	LPPER_IO_DATA ioInfo;
//	DWORD flags = 0;
//	int recvBytes = 0;
//	while (1)
//	{
//		//완료된 IO확인 
//		GetQueuedCompletionStatus(pComPort, &bytesTrans,
//			(PULONG_PTR)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
//		sock = handleInfo->hClntSock;
//
//		if (ioInfo->rwMode == READ)
//		{
//			if (bytesTrans == 0)    // EOF 전송 시
//			{
//				closesocket(sock);
//				free(handleInfo); free(ioInfo);
//				continue;
//			}
//
//			printf("[%d] message received! %s \n", handleInfo->nSess, ioInfo->buffer);
//			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
//			ioInfo->wsaBuf.len = bytesTrans;
//			ioInfo->rwMode = WRITE;
//			WSASend(sock, &(ioInfo->wsaBuf),
//				1, NULL, 0, &(ioInfo->overlapped), NULL);
//
//			ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
//			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
//			ioInfo->wsaBuf.len = BUF_SIZE;
//			ioInfo->wsaBuf.buf = ioInfo->buffer;
//			ioInfo->rwMode = READ;
//			WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
//		}
//		else
//		{
//			printf("message sent!\n");
//			free(ioInfo);
//		}
//	}
//	return 0;
//}

void ErrorHandling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}