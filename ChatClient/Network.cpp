#include "stdafx.h"
#include "Network.h"


Network::Network()
{
	printf("Network");
}


Network::~Network()
{
	printf("~Network");
}

int Network::Init(int nPort)
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

int Network::Run()
{
	RecvWork();
	SendWork();
	return 0;
}

int Network::RecvWork()
{
	std::thread t1 ([=]() {
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
				printf("SERVER fdNum %d \n", m_Read.fd_count);
				if (FD_ISSET(m_Read.fd_array[i], &m_Cpyread))
				{
					//��ȭ�ִ� ������ �������� == ���ο� ���� ��û 
					if (m_Read.fd_array[i] == m_ServSock)
					{
						printf("���ο� ���� ��û server  clnt = %d  servsock = %d \n", m_Read.fd_array[i], m_ServSock);
						OnConnect(m_Read.fd_array[i]);
					}

					else //read message
					{
						RecvMsg(i);


					}
				}
			}
		}
	});

	t1.detach();

	
	return 0;
}

int Network::SendWork()
{
	std::thread t1([=] {
		while (1)
		{
			if (m_SendQ.empty())
				continue;
			else
			{
				mu.lock();
				auto it = m_SendQ.begin();
				mu.unlock();

				if (!it->szbuf)
				{
					printf("SendWork szbuf NULL\n");
				}
				else
				{
					int nsend = send(it->nSock, it->szbuf, it->nsize, 0);
					if (nsend < 0)
					{
						printf("SendWork ????????\n");
					}
					printf("SendWork Send Msg socket %d buf %s size %d \n", it->nSock, it->szbuf, it->nsize);
					delete it->szbuf;
				}
				mu.lock();
				m_SendQ.erase(it);
				mu.unlock();
			}

		}
	});
	t1.detach();

	return 0;
}

int Network::ClientRecvWork()
{
	std::thread t1([=] {
 
		ClientRecvMsg(m_ServSock);
	
	
	});

	return 0;
}

int Network::ClientSendWork()
{
	std::thread t1([=] {




	});
	return 0;
}

int Network::RecvMsg(int nindex)
{
	char buf[BUF_SIZE] = { 0, };
	//��� ���� �ޱ� 
	int recvlen = recv(m_Read.fd_array[nindex], buf, PROTOCOL_HEADER_SIZE, 0);

	if (recvlen < 0)
	{
		if (errno == EAGAIN)
		{
			printf("----- read EAGAIN\n");
		}
		else
		{
			printf("recv ���� clnt = %d serv = %d\n", m_Read.fd_array[nindex], m_ServSock);
			OnDisconnect(m_Read.fd_array[nindex]);
		}

	}
	else if (recvlen == 0)
	{
		OnDisconnect(m_Read.fd_array[nindex]);
	}
	else
	{
		// ������ ó���� �� �ִ� �ּ� ���� ���� ���ϹǷ� parsing ����
		if (recvlen < PROTOCOL_HEADER_SIZE)
			return -1;

		//buf �Ľ��ؼ� � ���������� Ȯ�� 
		int nServiceCode;
		int nDataSize;
		void *szData = NULL;

		printf("recv msg = %s \n", buf);
		Parsing(buf, recvlen, '&', nServiceCode, nDataSize);

		char *databuf = new char[nDataSize];

		int nResult = RecvData(nindex, nDataSize, databuf);

		if (nResult <0)
		{
			//������Ͽ� �ִ� ���̸� ���� ó�� ���ֱ� 

			printf("������ ��Ŷ ���� \n");
			delete databuf;
			delete szData;
			return -1;

		}
		//ó���� 
		RecvHandler(m_Read.fd_array[nindex], nServiceCode, nDataSize, databuf);

		delete databuf;
		delete szData;

	}

	return 0;
	return 0;
}

int Network::ClientRecvMsg(int nSocket)
{
	char buf[BUF_SIZE] = { 0, };
	//��� ���� �ޱ� 
	int recvlen = recv(nSocket, buf, PROTOCOL_HEADER_SIZE, 0);

	if (recvlen < 0)
	{
		if (errno == EAGAIN)
		{
			printf("----- read EAGAIN\n");
		}
		else
		{
			printf("recv ���� clnt = %d serv = %d\n", nSocket, m_ServSock);
			OnDisconnect(nSocket);
		}

	}
	else if (recvlen == 0)
	{
		OnDisconnect(nSocket);
	}
	else
	{
		// ������ ó���� �� �ִ� �ּ� ���� ���� ���ϹǷ� parsing ����
		if (recvlen < PROTOCOL_HEADER_SIZE)
			return -1;

		//buf �Ľ��ؼ� � ���������� Ȯ�� 
		int nServiceCode;
		int nDataSize;
		void *szData = NULL;

		printf("recv msg = %s \n", buf);
		Parsing(buf, recvlen, '&', nServiceCode, nDataSize);

		char *databuf = new char[nDataSize];

		int nResult = ClientRecvData(nSocket, nDataSize, databuf);

		if (nResult <0)
		{
			//������Ͽ� �ִ� ���̸� ���� ó�� ���ֱ� 

			printf("������ ��Ŷ ���� \n");
			delete databuf;
			delete szData;
			return -1;

		}
		//ó���� 
		RecvHandler(nSocket, nServiceCode, nDataSize, databuf);

		delete databuf;
		delete szData;

	}

	return 0;

	return 0;
}

int Network::RecvData(int nindex, int nDataSize, char * databuf)
{

	int nlentotal = 0;
	int nTry = 0;

	while (nlentotal < nDataSize)
	{

		if (nTry == m_nMaxRecv)
			return -2;

		int nrecv = 0;
		nrecv = recv(m_Read.fd_array[nindex], databuf + nlentotal, nDataSize - nlentotal, 0);
		
		if (nrecv <= 0)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				//������ �ȿ°� ��ٷ��� 
				nTry++;
				printf("----- read EAGAIN \n");
			}

			else
				return -1;
		}
		else
			nlentotal += nrecv;

	}
	return 0;
}

int Network::ClientRecvData(int nSocket, int nDataSize, char * databuf)
{
	int nlentotal = 0;
	int nTry = 0;

	while (nlentotal < nDataSize)
	{

		if (nTry == m_nMaxRecv)
			return -2;

		int nrecv = 0;
		nrecv = recv(nSocket, databuf + nlentotal, nDataSize - nlentotal, 0);

		if (nrecv <= 0)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				//������ �ȿ°� ��ٷ��� 
				nTry++;
				printf("----- read EAGAIN \n");
			}

			else
				return -1;
		}
		else
			nlentotal += nrecv;

	}
	return 0;
}

int Network::SendMsg(int nSocket, int ServCode, int DataSize, char * szData, bool bDeleteData)
{

	if (!szData)
		return -1;

	int nTotMsgsize = PROTOCOL_HEADER_SIZE + DataSize;
	tMSG tMsg;
	char *szSendData = new char[nTotMsgsize+1];
	memset(szSendData, 0, nTotMsgsize);

	//sprintf �Ϸ��� ���ڿ� ���� ���� ���� +1 ������Ѵ�.
	//sprintf(szSendData, "%04d&%06d&%s", ServCode, DataSize , szData);

	sprintf(szSendData, "%04d&%06d&", ServCode, DataSize);
	memcpy(szSendData + PROTOCOL_HEADER_SIZE, szData, DataSize+1);

	tMsg.nSock = nSocket;
	tMsg.nsize = nTotMsgsize;
	tMsg.szbuf = szSendData;
	
	mu.lock();
	m_SendQ.push_back(tMsg);
	mu.unlock();

	//ť���� ������ ���� szSendData ���� 
	//delete szSendData;
	return 0;
}

int Network::Parsing(char * buf, int nrecvdata, char Sep, int & nServCode, int & nDataSize)
{
	if (!buf)
		return -1;


	//��ũ�����ڵ�&�����ͻ�����&������  
	//G&0001&000010&0123456789

	//�����ڵ�&�����ͻ�����&������  
	//0001&000010&0123456789
	char szServiceCode[100] = { 0, };
	char szDataSize[100] = { 0, };
	char *szRecvData;

	int curoffset = 0;
	//if (!(buf && 0x47))
	//{
	//	//����� ��ũ��Ʈ�� ���� 
	//	return -1;
	//}
	//curoffset++;
	//buf++;

	while (curoffset <nrecvdata)
	{

		memcpy(szServiceCode, buf, PROTOCOL_SERVIEC_TYPE);
		nServCode = atoi(szServiceCode);
		//int�� �ƴҶ� ����ó�� 

		buf += PROTOCOL_SERVIEC_TYPE + PROTOCOL_SEPARATOR;
		curoffset += PROTOCOL_SERVIEC_TYPE + PROTOCOL_SEPARATOR;


		memcpy(szDataSize, buf, PROTOCOL_MSG_SIZE);
		nDataSize = atoi(szDataSize);
		//int�� �ƴҶ� ����ó�� 

		buf += PROTOCOL_MSG_SIZE + PROTOCOL_SEPARATOR;
		curoffset += PROTOCOL_MSG_SIZE + PROTOCOL_SEPARATOR;
	}

	return 0;
 
}
