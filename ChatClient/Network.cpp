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

	//read에 servsock 소켓 등록 
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
				printf("SERVER fdNum %d \n", m_Read.fd_count);
				if (FD_ISSET(m_Read.fd_array[i], &m_Cpyread))
				{
					//변화있는 소켓이 서버소켓 == 새로운 연결 요청 
					if (m_Read.fd_array[i] == m_ServSock)
					{
						printf("새로운 연결 요청 server  clnt = %d  servsock = %d \n", m_Read.fd_array[i], m_ServSock);
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
	//헤더 먼저 받기 
	int recvlen = recv(m_Read.fd_array[nindex], buf, PROTOCOL_HEADER_SIZE, 0);

	if (recvlen < 0)
	{
		if (errno == EAGAIN)
		{
			printf("----- read EAGAIN\n");
		}
		else
		{
			printf("recv 오류 clnt = %d serv = %d\n", m_Read.fd_array[nindex], m_ServSock);
			OnDisconnect(m_Read.fd_array[nindex]);
		}

	}
	else if (recvlen == 0)
	{
		OnDisconnect(m_Read.fd_array[nindex]);
	}
	else
	{
		// 데이터 처리할 수 있는 최소 단위 만족 못하므로 parsing 안함
		if (recvlen < PROTOCOL_HEADER_SIZE)
			return -1;

		//buf 파싱해서 어떤 데이터인지 확인 
		int nServiceCode;
		int nDataSize;
		void *szData = NULL;

		printf("recv msg = %s \n", buf);
		Parsing(buf, recvlen, '&', nServiceCode, nDataSize);

		char *databuf = new char[nDataSize];

		int nResult = RecvData(nindex, nDataSize, databuf);

		if (nResult <0)
		{
			//유저목록에 있는 놈이면 삭제 처리 해주기 

			printf("비정상 패킷 버림 \n");
			delete databuf;
			delete szData;
			return -1;

		}
		//처리부 
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
	//헤더 먼저 받기 
	int recvlen = recv(nSocket, buf, PROTOCOL_HEADER_SIZE, 0);

	if (recvlen < 0)
	{
		if (errno == EAGAIN)
		{
			printf("----- read EAGAIN\n");
		}
		else
		{
			printf("recv 오류 clnt = %d serv = %d\n", nSocket, m_ServSock);
			OnDisconnect(nSocket);
		}

	}
	else if (recvlen == 0)
	{
		OnDisconnect(nSocket);
	}
	else
	{
		// 데이터 처리할 수 있는 최소 단위 만족 못하므로 parsing 안함
		if (recvlen < PROTOCOL_HEADER_SIZE)
			return -1;

		//buf 파싱해서 어떤 데이터인지 확인 
		int nServiceCode;
		int nDataSize;
		void *szData = NULL;

		printf("recv msg = %s \n", buf);
		Parsing(buf, recvlen, '&', nServiceCode, nDataSize);

		char *databuf = new char[nDataSize];

		int nResult = ClientRecvData(nSocket, nDataSize, databuf);

		if (nResult <0)
		{
			//유저목록에 있는 놈이면 삭제 처리 해주기 

			printf("비정상 패킷 버림 \n");
			delete databuf;
			delete szData;
			return -1;

		}
		//처리부 
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
				//데이터 안온거 기다려줌 
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
				//데이터 안온거 기다려줌 
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

	//sprintf 하려면 문자열 종료 문자 들어가서 +1 해줘야한다.
	//sprintf(szSendData, "%04d&%06d&%s", ServCode, DataSize , szData);

	sprintf(szSendData, "%04d&%06d&", ServCode, DataSize);
	memcpy(szSendData + PROTOCOL_HEADER_SIZE, szData, DataSize+1);

	tMsg.nSock = nSocket;
	tMsg.nsize = nTotMsgsize;
	tMsg.szbuf = szSendData;
	
	mu.lock();
	m_SendQ.push_back(tMsg);
	mu.unlock();

	//큐에서 보내고 나서 szSendData 삭제 
	//delete szSendData;
	return 0;
}

int Network::Parsing(char * buf, int nrecvdata, char Sep, int & nServCode, int & nDataSize)
{
	if (!buf)
		return -1;


	//싱크서비스코드&데이터사이즈&데이터  
	//G&0001&000010&0123456789

	//서비스코드&데이터사이즈&데이터  
	//0001&000010&0123456789
	char szServiceCode[100] = { 0, };
	char szDataSize[100] = { 0, };
	char *szRecvData;

	int curoffset = 0;
	//if (!(buf && 0x47))
	//{
	//	//헤더상에 싱크파트가 없음 
	//	return -1;
	//}
	//curoffset++;
	//buf++;

	while (curoffset <nrecvdata)
	{

		memcpy(szServiceCode, buf, PROTOCOL_SERVIEC_TYPE);
		nServCode = atoi(szServiceCode);
		//int값 아닐때 예외처리 

		buf += PROTOCOL_SERVIEC_TYPE + PROTOCOL_SEPARATOR;
		curoffset += PROTOCOL_SERVIEC_TYPE + PROTOCOL_SEPARATOR;


		memcpy(szDataSize, buf, PROTOCOL_MSG_SIZE);
		nDataSize = atoi(szDataSize);
		//int값 아닐때 예외처리 

		buf += PROTOCOL_MSG_SIZE + PROTOCOL_SEPARATOR;
		curoffset += PROTOCOL_MSG_SIZE + PROTOCOL_SEPARATOR;
	}

	return 0;
 
}
