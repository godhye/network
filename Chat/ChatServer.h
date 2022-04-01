#pragma once
#include "���.h"
#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <Windows.h>
#include <vector>
#include <map>

typedef struct sUser {

	//����
	SOCKET socket;
	//���忩��
	bool bOwner = false;
	//�г���
	std::string strNick;
	//�����ѹ�ѹ�
	int nRoomNum = -1;
} tUser;

typedef struct sRoom {
	//���ȣ
	int nRoomNum =-1;
	//����
	std::string strTitle;
	//�ο��� 
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


	int RecvMsg(); //���� ������ Ÿ�Կ� ���� �б� ó�� 
	int SendMsg(int nSocket, int ServCode , int DataSize , char* szData , bool bDeleteData = true); //������ ���� 
	int Parsing(   char* buf , char Sep, int &nServCode, int &nDataSize, char** szData);
	int RecvHandler(  int nServCode, int nDataSize, char* szData);
	//servsock , addr
	SOCKET m_ServSock, m_CpyServ, m_ClntSock;
	SOCKADDR_IN m_Servaddr, m_Clntaddr;

	//FDSET  ���� ����
	fd_set m_Read, m_Cpyread;

	//��������Ʈ ����Ű / ��������
	std::map<unsigned int , tUser> m_mapUser; 
	//ä�ù渮��Ʈ ä�ù��ȣ / ä�ù�����
	std::map<int, tRoom> m_mapRoom;

	int nChatRoom = 0;

};

