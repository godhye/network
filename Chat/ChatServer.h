#pragma once
#include "헤더.h"
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <Windows.h>
#include <vector>
#include <map>

typedef struct sUser {

	//소켓
	SOCKET socket;
	//방장여부
	bool bOwner = false;
	//닉네임
	std::string strNick;
	//참가한방넘버
	int nRoomNum = -1;
} tUser;

typedef struct sRoom {
	//방번호
	int nRoomNum =-1;
	//제목
	std::string strTitle;
	//인원수 
	int nTotal = -1;
}tRoom;


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
	int SendMsg(int nSocket, int ServCode , int DataSize , char* szData , bool bDeleteData = true); //데이터 전송 
	int Parsing(   char* buf , char Sep, int &nServCode, int &nDataSize, char** szData);
	int RecvHandler(  int nServCode, int nDataSize, char* szData);
	//servsock , addr
	SOCKET m_ServSock, m_CpyServ, m_ClntSock;
	SOCKADDR_IN m_Servaddr, m_Clntaddr;

	//FDSET  소켓 보관
	fd_set m_Read, m_Cpyread;

	//유저리스트 세션키 / 유저정보
	std::map<unsigned int , tUser> m_mapUser; 
	//채팅방리스트 채팅방번호 / 채팅방정보
	std::map<int, tRoom> m_mapRoom;

	int nChatRoom = 0;

};

