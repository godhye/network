#pragma once
#include "헤더.h"
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <Windows.h>
#include <vector>
#include <map>
#include <set>

class ChatServer
{
public:
	ChatServer();
	~ChatServer();

	int Init(int nPort);
	int Work();
	int Clean();

	
	int GetUserIndex(const SOCKET s);
	int BroadCast(int nRoomnum , int msgtype , char* buf);

private:


	int RecvMsg(); //받은 데이터 타입에 따른 분기 처리 
	int SendMsg(int nSocket,int nChatRoom, int ServCode , int DataSize , char* szData , bool bDeleteData = true); //데이터 전송 
	int OpenRoom(int nSocket, char* szTitle, char* szPassword, int &nRoomNum);
	int JoinRoom(int nSocket, int nRoomNum,char* szPassword );
	int QuitRoom(int nRoomNum);
	int SendResult(int nSocket, int result);
	int CloseConnect(int nSocket);
	int OpenConnect(int nSocket);
	int Parsing(   char* buf , int recvdata, char Sep, int &nServCode, int &nDataSize, char** szData);
	int RecvHandler(int nSocket, int nServCode, int nDataSize, char* szData);


	int RecvProtocolOPEN(int nSocket, char* szData);
	int RecvProtocolJOIN(int nSocket, char* szData);
	int RecvProtocolMSG(int nSocket, char* szData);
	int RecvProtocolUPDATE(int nSocket, char* szData);
	int RecvProtocolFILE(int nSocket, char* szData);
	int RecvProtocolQUIT(int nSocket, char* szData);

	int GetJoinedChatRoomNum(int nSocket , int &nRoomNum);

	//servsock , addr
	SOCKET m_ServSock, m_CpyServ, m_ClntSock;
	SOCKADDR_IN m_Servaddr, m_Clntaddr;

	//FDSET  소켓 보관
	fd_set m_Read, m_Cpyread;

	//유저리스트 세션키 / 유저정보
	std::map<unsigned int , tUser> m_mapUser; 
	//채팅방리스트 채팅방번호 / 채팅방정보
	std::map<int, tRoom> m_mapRoom;

	int m_nChatRoom = 0;

};

