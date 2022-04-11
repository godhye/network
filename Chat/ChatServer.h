#pragma once
#include "헤더.h"
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include "Network.h"
#include <Windows.h>
#include <vector>
#include <map>
#include <set>

class ChatServer : public  Network
{
public:
	ChatServer();
	~ChatServer();

	//int Init(int nPort);
	//int Work();
	int Clean();

	
	int GetUserIndex(const SOCKET s);
	int BroadCast(int nRoomnum , int msgtype , char* buf);

private:


	 int OpenRoom(int nSocket, char* szTitle, char* szPassword, int &nRoomNum);
	int JoinRoom(int nSocket, int nRoomNum,char* szPassword );
	int QuitRoom(int nRoomNum);
	int SendResult(int nSocket, int nServicetype, int nDatasize, int nResult);
	int CloseConnect(int nSocket);
	int SetNewUser(int nSocket);

	 int OnConnect(int nSocket);
	int OnDisconnect(int nSocket);

	 int RecvHandler(int nSocket, int nServCode, int nDataSize, char* szData);


	int RecvProtocolOPEN(int nSocket, char* szData);
	int RecvProtocolJOIN(int nSocket, char* szData);
	int RecvProtocolMSG(int nSocket, char* szData);
	int RecvProtocolUPDATE(int nSocket, char* szData);
	int RecvProtocolFILE(int nSocket, char* szData);
	int RecvProtocolQUIT(int nSocket, char* szData);

	int GetJoinedChatRoomNum(int nSocket , int &nRoomNum);

 

	//유저리스트 세션키 / 유저정보
	std::map<unsigned int , tUser> m_mapUser; 
	//채팅방리스트 채팅방번호 / 채팅방정보
	std::map<int, tRoom> m_mapRoom;

	int m_nChatRoom = 0;

};

