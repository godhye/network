// socket.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <WinSock2.h>
#include <thread>


 
void mythread(HANDLE* h) {
	while (1) {
		

	}
	 
}
int main()
{
    std::cout << "Hello World!\n";
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}

	 
	// 서버정보 객체설정
	SOCKADDR_IN serverAddr[2];
	for (int i = 0; i < 2; i++) {

		memset(&serverAddr[i], 0, sizeof(SOCKADDR_IN));
		serverAddr[i].sin_family = PF_INET;
		serverAddr[i].sin_port = htons(16000);
		serverAddr[i].sin_addr.S_un.S_addr = inet_addr("118.217.182.242");
	
	}

	serverAddr[1].sin_port = htons(19000);

	SOCKADDR_IN clientAddr[2];
	 
	int addrLen = sizeof(SOCKADDR_IN);
	  
	SOCKET clientSocket[2];
	 
	WSAEVENT hevent[2];

	 for (int i = 0; i < 2; i++) {
		 clientSocket[i] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
		 memset(&clientAddr[i], 0, addrLen);
		 hevent[i] = WSACreateEvent();
	 }
	 
	for(int i =0 ; i< 2 ; i++)
	{
		 int nresult =  connect(clientSocket[i], (struct sockaddr *)&serverAddr[i], addrLen);
		if (clientSocket[i] == INVALID_SOCKET || nresult == -1 )
		{
			printf("Error - Accept Failure\n");
			return 1;
		}

		printf("Connect\n");
		WSAEventSelect(clientSocket[i], hevent[i], FD_CLOSE);
		 
 	}

	while (1) {

		//슬레이브리스트 가져와서 셋팅 
		//맵에 ip:port key로 해서 소켓 삽입
		DWORD dint = WaitForMultipleObjects(2, hevent, false, INFINITE);

		switch (dint)
		{
		case WAIT_OBJECT_0 + 0:
			printf("0 index");
			ResetEvent(hevent[0]);
			break;
		case WAIT_OBJECT_0 + 1:

			ResetEvent(hevent[1]);
			printf("1 index");
			break;
		default:
			break;
		}
		printf("close!!!\n");
	}

	// 6-2. 리슨 소켓종료
	for(int i =0 ; i <2 ; i++)
	closesocket(clientSocket[i]);

	// Winsock End
	WSACleanup();

	return 0;



}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
