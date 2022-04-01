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

	//read�� servsock ���� ��� 
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

		//m_Cpyread �ݵ�� ����� 
		if ((fdNum = select(0, &m_Cpyread, 0, 0, &timeout)) == SOCKET_ERROR)
		{
			printf("SERVER fdNum ERROR \n");
			break;
		}
		//ȣ�� ��� Ȯ�� 
		//��ȭ�� ���� ������ 
		if (fdNum == 0)
		{
 
			continue;
		}

		 
		for (int i = 0; i < m_Read.fd_count; i++)
		{
			printf("SERVER fdNum %d \n" , m_Read.fd_count);
			if (FD_ISSET(m_Read.fd_array[i], &m_Cpyread))
			{
				//��ȭ�ִ� ������ �������� == ���ο� ���� ��û 
				if (m_Read.fd_array[i] == m_ServSock)
				{
					printf("���ο� ���� ��û server  clnt = %d  servsock = %d \n", m_Read.fd_array[i], m_ServSock);
					int adSz = sizeof(m_Clntaddr);
					m_ClntSock = accept(m_ServSock, (SOCKADDR*)&m_Clntaddr, &adSz);
					//���� ����� Ŭ���̾�Ʈ���� ���
					FD_SET(m_ClntSock, &m_Read);

					//���ο� ���� ����
					tUser user;// = new tUser;
					user.socket = m_ClntSock;
			
					m_mapUser.insert(std::pair<unsigned int , tUser>(m_ClntSock , user));

					printf("new user %d\n", m_ClntSock);
				}

				else //read message
				{


					char buf[BUF_SIZE+1] = {0,};
					int recvlen = recv(m_Read.fd_array[i], buf, BUFSIZ , 0);

					//buf �Ľ��ؼ� � ���������� Ȯ�� 
					

					if (recvlen < 0)
					{
						printf("recv ���� clnt = %d serv = %d\n", m_Read.fd_array[i], m_ServSock);
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
						printf("��������  clnt = %d m_ServSock =%d \n", m_Read.fd_array[i], m_ServSock);
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
						//���� �濡 �ִ� �ֵ����� braodcast
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
	//wsa ����
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


	//�����ڵ�&�����ͻ�����&������  
	//0001&000010&0123456789
	char szServiceCode[100] = { 0, };
	char szDataSize[100] = { 0, };
	
	char *szRecvData;

	size_t offsetStart = 0;;
	size_t offsetEnd = 0;;


	while (strlen(buf)>0)
	{
		//���� ������ ������ ó�� �ؾ��� �ٵ� ��� �ؾ����� �𸣰��� ��
		memcpy(szServiceCode, buf, sizeof(char) *4);
		nServCode = atoi(szServiceCode);
		buf += 4 + 1 ;
		//buf += sizeof(char); // ������ 

		memcpy(szDataSize, buf, sizeof(char) * 6);
		nDataSize = atoi(szDataSize);
		buf += 6 +1 ;
		//buf += sizeof(char); // ������ 

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
