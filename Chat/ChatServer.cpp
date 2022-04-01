#include "stdafx.h"
#include "ChatServer.h"


ChatServer::ChatServer()
{
}


ChatServer::~ChatServer()
{
}

int ChatServer::Init(int nPort)
{
	m_ServSock = socket(PF_INET, SOCK_STREAM, 0);
	m_CpyServ = m_ServSock;
	m_Servaddr.sin_family = AF_INET;
	m_Servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_Servaddr.sin_port = nPort;

	int sockopt = 1;
	int nResult;
	setsockopt(m_ServSock, SOL_SOCKET, SO_REUSEADDR, (char*)&sockopt, sizeof(sockopt));
	nResult = bind(m_ServSock, (sockaddr*)&m_Servaddr, sizeof(m_Servaddr));
	if (nResult < 0)
		return -1;
	nResult = listen(m_ServSock, 5);

	if (nResult < 0)
		return -1;

	FD_ZERO(&m_Read);

	//read에 servsock 소켓 등록 
	FD_SET(m_ServSock, &m_Read);

	return 0;
}

int ChatServer::Work()
{
	TIMEVAL timeout;
	printf("SERVER RUN \n");

	while (1)
	{
		_sleep(100);
		m_Cpyread = m_Read;
		timeout.tv_sec = 1;
		timeout.tv_usec = 100000;
		int fdNum;

		//m_Cpyread 반드시 써야함 
		if ((fdNum = select(0, &m_Cpyread, 0, 0, &timeout)) == SOCKET_ERROR)
		{
			printf("SERVER fdNum ERROR \n");
			break;
		}
		//호출 결과 확인 
		//변화된 소켓 없을때 
		if (fdNum == 0)
		{
 
			continue;
		}

		 
		for (int i = 0; i < m_Read.fd_count; i++)
		{
			printf("SERVER fdNum %d \n" , m_Read.fd_count);
			if (FD_ISSET(m_Read.fd_array[i], &m_Cpyread))
			{
				//변화있는 소켓이 서버소켓 == 새로운 연결 요청 
				if (m_Read.fd_array[i] == m_ServSock)
				{
					printf("새로운 연결 요청 server  clnt = %d  servsock = %d \n", m_Read.fd_array[i], m_ServSock);
					int adSz = sizeof(m_Clntaddr);
					m_ClntSock = accept(m_ServSock, (SOCKADDR*)&m_Clntaddr, &adSz);
					//새로 연결된 클라이언트소켓 등록
					FD_SET(m_ClntSock, &m_Read);

					//새로운 유저 생성
					tUser user;// = new tUser;
					user.socket = m_ClntSock;
			
					m_mapUser.insert(std::pair<unsigned int , tUser>(m_ClntSock , user));

					printf("new user %d\n", m_ClntSock);
				}

				else //read message
				{


					char buf[BUF_SIZE+1] = {0,};
					int recvlen = recv(m_Read.fd_array[i], buf, BUFSIZ , 0);

					//buf 파싱해서 어떤 데이터인지 확인 
					

					if (recvlen < 0)
					{
						printf("recv 오류 clnt = %d serv = %d\n", m_Read.fd_array[i], m_ServSock);
						FD_CLR(m_Read.fd_array[i], &m_Read);
						auto it = m_mapUser.find(m_Read.fd_array[i]);
						if (it == m_mapUser.end())
						{
							printf("userlist find error");
						}
						else
						{
							printf("userlist erase");
							m_mapUser.erase(it);
						}
						closesocket(m_Cpyread.fd_array[i]);
					}

					else if (recvlen == 0)
					{


						FD_CLR(m_Read.fd_array[i], &m_Read);
						auto it = m_mapUser.find(m_Read.fd_array[i]);
						if (it == m_mapUser.end())
						{
							printf("userlist find error");
						}
						else
						{
							printf("userlist erase");
							m_mapUser.erase(it);
						}
						closesocket(m_Cpyread.fd_array[i]);
						printf("연결종료  clnt = %d m_ServSock =%d \n", m_Read.fd_array[i], m_ServSock);
					}

					else
					{
						int nServiceCode;
						int nDataSize;
						char *szData = NULL;
						buf[recvlen] = '\0';
						printf("recv msg = %s \n", buf);
						Parsing(buf , '&' , nServiceCode , nDataSize, &szData);
						
						RecvHandler(  nServiceCode, nDataSize, szData);
						//같은 방에 있는 애들한테 braodcast
					}



				}
			}
		}
	}
	return 0;
}

int ChatServer::Clean()
{

	closesocket(m_ClntSock);
	closesocket(m_ServSock);
	//wsa 정리
	WSACleanup();
	return 0;
}

int ChatServer::SendMsg(int nSock , int ServCode, const int DataSize, char * szData, bool bDeleteData)
{

	char *szSendData = new char[DataSize + (sizeof(int) + sizeof(char)) * 2];
	memset(szSendData, 0, strlen(szSendData));

	sprintf(szSendData,"%d&%d&%s", ServCode, DataSize, szData);
	
	send(nSock, szSendData, strlen( szSendData), NULL);

	return 0;
}

int ChatServer::Parsing(char * buf, char Sep, int &nServCode, int &nDataSize , char** szData)
{
	if (!buf)
		return -1;


	//서비스코드&데이터사이즈&데이터  
	//0001&000010&0123456789
	char szServiceCode[100] = { 0, };
	char szDataSize[100] = { 0, };
	
	char *szRecvData;

	size_t offsetStart = 0;;
	size_t offsetEnd = 0;;


	while (strlen(buf)>0)
	{
		//남은 데이터 있을때 처리 해야함 근데 어떻게 해야할지 모르겟음 ㅋ
		memcpy(szServiceCode, buf, sizeof(char) *4);
		nServCode = atoi(szServiceCode);
		buf += 4 + 1 ;
		//buf += sizeof(char); // 구분자 

		memcpy(szDataSize, buf, sizeof(char) * 6);
		nDataSize = atoi(szDataSize);
		buf += 6 +1 ;
		//buf += sizeof(char); // 구분자 

		char *szData1 = new char[sizeof(char)*nDataSize];
		memcpy(szData1, buf, nDataSize);
		*szData = szData1;
		buf += nDataSize;
	 
	}

	

 

	return 0;
}

int ChatServer::RecvHandler(int nServCode, int nDataSize, char * szData)
{
	switch (nServCode)
	{
	case PROTOCOL_JOIN :
		printf("%d %d %s\n", nServCode, nDataSize, szData);
		return 1;

	default:
		break;
	}
	return 0;
}
