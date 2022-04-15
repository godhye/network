#include "stdafx.h"
#include "ChatClient.h"


ChatClient::ChatClient()
{
}


ChatClient::~ChatClient()
{
 
}

int ChatClient::Init()
{

	m_ServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ServSock == INVALID_SOCKET)
	{
		puts("hServ error");
		return -1;
	}

	//구조체 값 한꺼번에 초기화
	memset(&m_Servaddr, 0, sizeof(SOCKADDR));
	//AF_INET = ipv4  주소체계 프로토콜
	m_Servaddr.sin_family = AF_INET;
	m_Servaddr.sin_addr.s_addr = inet_addr(ADDR);
	m_Servaddr.sin_port = USE_PORT;

	//connect
	int nresult = connect(m_ServSock, (SOCKADDR*)&m_Servaddr, sizeof(m_Servaddr));
	if (nresult == SOCKET_ERROR)
	{
		puts("connect error");
		return -1;
	}

}

int ChatClient::Run()
{
	std::thread t1([=] {
		ClientRecvWork();
	});
	t1.detach();

	std::thread t2([=] {
		SendWork();
	});
	t2.detach();



	return 0;
}
