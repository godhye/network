#include "stdafx.h"
#include "ChatServer.h"


ChatServer::ChatServer()
{
	printf("ChatServer");
}


ChatServer::~ChatServer()
{
	printf("~ChatServer");
}
 
int ChatServer::Clean()
{

	closesocket(m_ClntSock);
	closesocket(m_ServSock);
	//wsa 정리
	WSACleanup();
	return 0;
}
//
//int ChatServer::RecvMsg(int nindex)
//{
//	char buf[BUF_SIZE] = { 0, };
//	//헤더 먼저 받기 
//	int recvlen = recv(m_Read.fd_array[nindex], buf, PROTOCOL_HEADER_SIZE, 0);
//	 
//	if (recvlen < 0)
//	{
//		if (errno == EAGAIN)
//		{
//			printf("----- read EAGAIN\n");
//		}
//		else
//		{
//			printf("recv 오류 clnt = %d serv = %d\n", m_Read.fd_array[nindex], m_ServSock);
//			CloseConnect(m_Read.fd_array[nindex]);
//		}
//		
//	}
//	else if (recvlen == 0)
//	{
//		CloseConnect(m_Read.fd_array[nindex]);
//	}
//	else
//	{
//		// 데이터 처리할 수 있는 최소 단위 만족 못하므로 parsing 안함
//		if (recvlen < PROTOCOL_HEADER_SIZE)
//			return -1;
//
//		//buf 파싱해서 어떤 데이터인지 확인 
//		int nServiceCode;
//		int nDataSize;
//		void *szData = NULL;
//
//		printf("recv msg = %s \n", buf);
//		Parsing(buf, recvlen, '&', nServiceCode, nDataSize);
//
//		char *databuf = new char[nDataSize];
//
//		int nResult = RecvData(nindex, nDataSize, databuf);
//	
//		if (nResult <0)
//		{
//			//유저목록에 있는 놈이면 삭제 처리 해주기 
//			
//			printf("비정상 패킷 버림 \n");
//			delete databuf;
//			delete szData;
//			return -1;
//
//		}
//		//처리부 
//		RecvHandler(m_Read.fd_array[nindex], nServiceCode, nDataSize, databuf);
//
//		delete databuf;
//		delete szData;
//	 
//	}
//
//	return 0;
//}
//
//int ChatServer::RecvData(int nindex, int nDataSize, char * databuf)
//{
//	int nlentotal = 0;
//	int nTry = 0;
//
//		while (nlentotal < nDataSize )
//		{
//
//			if (nTry == m_nMaxRecv)
//				return -2;
//
//			int nrecv = 0;
//			nrecv= recv(m_Read.fd_array[nindex], databuf + nlentotal, nDataSize - nlentotal, 0);
//			nTry++;
//			if (nrecv <= 0)
//			{
//				if (WSAGetLastError() == WSAEWOULDBLOCK)
//				{
//					printf("----- read EAGAIN \n");
//				}
//
//				else
//					return -1;
//			}
//			else
//				nlentotal += nrecv;
//			 
//		}
//	  
//	return 0;
//}
//
//int ChatServer::SendMsg(int nSock , int nChatRoom, int ServCode, const int DataSize, char * szData, bool bDeleteData)
//{
//
//	int nTotMsgsize = PROTOCOL_SEPARATOR * 2 + PROTOCOL_SERVIEC_TYPE + PROTOCOL_MSG_SIZE + DataSize+1;
//
//	char *szSendData = new char[nTotMsgsize];
//	memset(szSendData, 0, nTotMsgsize);
//
//	sprintf(szSendData,"%04d&%06d&%s", ServCode, DataSize, szData);
//	
//	//참가한 방 번호 가져오기
//	auto it = m_mapRoom.find(nChatRoom);
//	if (it == m_mapRoom.end())
//	{
//		printf("참가한 방 번호 오류");
//		return -1;
//	}
//
//	
//	for(auto it2 = it->second.setUsers.begin();it2 != it->second.setUsers.end() ; it2++)
//	send(nSock, szSendData, strlen(szSendData), NULL);
//
//	delete szSendData;
//	return 0;
//}

int ChatServer::OpenRoom(int nSocket, char * szTitle, char * szPassword, int & nRoomNum)
{
	if (!szTitle || !szPassword)
		return -1;

	//채팅방 생성
	tRoom newRoom;
	newRoom.nRoomNum = m_nChatRoom;
	strncpy(newRoom.szTitle, szTitle , 100);
	strncpy(newRoom.szPassword, szPassword, 100);
	newRoom.nTotal = 0;
	newRoom.setUsers.insert(nSocket);
	newRoom.nOwnerNum = nSocket;
	nRoomNum = newRoom.nRoomNum;
	
	m_mapRoom.insert(std::pair<int , tRoom>(nRoomNum, newRoom));
	m_nChatRoom++;
	printf("OpenRoom %d ", newRoom.nRoomNum);

	return 0;
}

int ChatServer::JoinRoom(int nSocket, int nRoomNum, char * szPassword)
{

	auto it = m_mapRoom.find(nRoomNum);

	if (it == m_mapRoom.end())
	{
		printf("ERROR JoinRoom 1\n");
		return -1;
	}

	if (it->second.bPasswordSet)
	{
		if (strcmp(it->second.szPassword, szPassword) != 0)
		{
			printf("ERROR JoinRoom szPassword 2\n");
			return -2;
		}
		else
		{
			//유저 추가
			tUser newUser;
			newUser.nRoomNum = nRoomNum;
			newUser.h_socket = nSocket;
			m_mapUser.insert(std::pair<int, tUser>(nSocket, newUser));

			//참가한 방에 추가 
			it->second.setUsers.insert(nSocket);
			it->second.nTotal++;

		}
	}
	
	return 0;
}

int ChatServer::QuitRoom(int nRoomNum)
{
	return 0;
}

int ChatServer::SendResult(int nSocket, int nServicetype , int nDatasize , int nResult)
{
	TCHAR buf[BUF_SIZE] = { 0, };
	TCHAR Sendbuf[BUF_SIZE] = { 0, };

	sprintf(Sendbuf, "%04d&%06d&%04d", nServicetype, 12+sizeof(int),nResult);
	int len = send(nSocket, Sendbuf, 12 + sizeof(int), 0);
	return 0;
}

int ChatServer::CloseConnect(int nSocket)
{
	FD_CLR(nSocket, &m_Read);
	auto it = m_mapUser.find(nSocket);
	if (it == m_mapUser.end())
	{
		printf("userlist find error");
	}
	else
	{
		printf("userlist erase");
		m_mapUser.erase(it);
	}
	closesocket(nSocket);

	printf("연결종료  clnt = %d m_ServSock =%d \n", nSocket, m_ServSock);
	return 0;
}
 
 

int ChatServer::OnConnect(int nSocket)
{
	int adSz = sizeof(m_Clntaddr);
	m_ClntSock = accept(m_ServSock, (SOCKADDR*)&m_Clntaddr, &adSz);
	//새로 연결된 클라이언트소켓 등록
	FD_SET(m_ClntSock, &m_Read);

	//클라이언트 소켓은 논블로킹 소켓 설정

	ULONG iMode = 1;
	ioctlsocket(m_ClntSock, FIONBIO, &iMode);
	//새로운 유저 생성
	tUser user;// = new tUser;
	user.h_socket = m_ClntSock;

	m_mapUser.insert(std::pair<unsigned int, tUser>(m_ClntSock, user));

	printf("new user %d\n", m_ClntSock);
	 
	return 0;
}

int ChatServer::OnDisconnect(int nSocket)
{
	FD_CLR(nSocket, &m_Read);
	auto it = m_mapUser.find(nSocket);
	if (it == m_mapUser.end())
	{
		printf("userlist find error");
	}
	else
	{
		printf("userlist erase");
		m_mapUser.erase(it);
	}
	closesocket(nSocket);

	printf("연결종료  clnt = %d m_ServSock =%d \n", nSocket, m_ServSock);
	return 0;
	 
}

 
int ChatServer::RecvHandler(int nSocket,int nServCode, int nDataSize, char * szData)
{
	switch (nServCode)
	{
	case PROTOCOL_OPEN:
		printf("%d %d %s\n", nServCode, nDataSize, szData);
		if (RecvProtocolOPEN(nSocket, szData) == -1)
			printf("ERROR");
		else
			SendResult(nSocket, nServCode, sizeof(int), 1);
		break;

	case PROTOCOL_JOIN:
		printf("%d %d %s\n", nServCode, nDataSize, szData);
		RecvProtocolJOIN(nSocket, szData);
		break;
	case PROTOCOL_MSG:
		printf("%d %d %s\n", nServCode, nDataSize, szData);
		RecvProtocolMSG(nSocket, szData);
		break;
	case PROTOCOL_UPDATE:
		printf("%d %d %s\n", nServCode, nDataSize, szData);
		break;
	case PROTOCOL_FILE:
		printf("%d %d %s\n", nServCode, nDataSize, szData);
		break;
	case PROTOCOL_QUIT:
		printf("%d %d %s\n", nServCode, nDataSize, szData);
		break;
	default:
		break;
	}
	return 0;
}

int ChatServer::RecvProtocolOPEN(int nSocket, char * szData)
{
	if (!szData)
		return -1;

	sDATAOPEN *sData = (sDATAOPEN*)szData;
	int nRoom;
	if (OpenRoom(nSocket, sData->szTitle, sData->szPassword, nRoom) < 0)
	{	
		printf("ERROR OpenRoom \n");
		return -1;
	}


	return 0;
}

int ChatServer::RecvProtocolJOIN(int nSocket, char * szData)
{
	if (!szData)
		return -1;

	sDATAJOIN *sData = (sDATAJOIN*)szData;
	
	if (JoinRoom(nSocket, sData->nJoinRoom, sData->szPassword ) < 0)
	{
		printf("ERROR JoinRoom 3\n");
		return -1;
	}
	return 0;
}

int ChatServer::RecvProtocolMSG(int nSocket, char * szData)
{
	int nChatRoom;
	int nresult = GetJoinedChatRoomNum(nSocket , nChatRoom);

	if (nChatRoom > 0)
	{
		auto it = m_mapRoom.find(nChatRoom);
		if (it == m_mapRoom.end())
		{
			printf("ERROR RecvProtocolMSG 3\n");
			return -1;
		}
		else
		{
			//braodcast 
			for (auto itset = it->second.setUsers.begin(); itset != it->second.setUsers.end(); itset++)
			{
				send(*itset, szData, strlen(szData), 0);
			}

		}


	}

	

	return 0;
}

int ChatServer::GetJoinedChatRoomNum(int nSocket, int &nRoomNum)
{
	auto it = m_mapUser.find(nSocket);
	if (it == m_mapUser.end())
	{
		//참여한 방 없음 
		return -1;
	}
	nRoomNum = it->second.nRoomNum;

	return 0;
}
