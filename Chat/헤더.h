#pragma once

typedef struct sDATAEXP 
{
	size_t nfilesize;
	char szFilename[100];

}tDATAEXP;

/* ��Ŷ ���� 

����Ÿ��&�����ͻ�����&������
04%d&06d&%s

*/


#define PROTOCOL_SERVIEC_TYPE 4
#define PROTOCOL_SEPARATOR 1
#define PROTOCOL_MSG_SIZE 6


/*
//����Ÿ�� 
0 //PROTOCOL_OPEN			// ä�ù氳��-���� , ��й�ȣ  
0 //PROTOCOL_JOIN			// ���� -  ��й�ȣ
0 //PROTOCOL_MSG			// ä�� ���� 
1 //PROTOCOL_UPDATE			// ä�ù����� , ��й�ȣ 
2 //PROTOCOL_FILE			// ���ϸ� , ���� ������ 
3 //PROTOCOL_QUIT			// ����

*/

//�޼�������  


typedef struct sDATAJOIN
{
	int nJoinRoom; //�������ϴ� ä�ù� 
	char szPassword[BUFSIZ] = { 0, }; // ��й�ȣ 

}tDATAJOIN;

typedef struct sDATAOPEN
{
	char szPassword[100] = { 0, }; // ��й�ȣ 
	char szTitle[100];
	int ntest = 3;
}tDATAOPEN;

typedef struct sDATAMSG
{
	int nJoinRoom; //�������ϴ� ä�ù� 
	char szPassword[BUFSIZ] = { 0, }; // ��й�ȣ 

}tDATAMSG;

#define PROTOCOL_OPEN 0 
#define PROTOCOL_JOIN 1
#define PROTOCOL_MSG 2
#define PROTOCOL_UPDATE 3 
#define PROTOCOL_FILE 4
#define PROTOCOL_KEEPALIVE 5
#define PROTOCOL_QUIT 6

#define BUF_SIZE 1024 // 1K



typedef struct sUser {

	//����
	unsigned int  h_socket;
	//�г���
	std::string strNick;
	//�����ѹ�ѹ�
	int nRoomNum = -1;
} tUser;

typedef struct sRoom {
	//���ȣ
	int nRoomNum = -1;
	//����
	char szTitle[100] = { 0, };
	//�ο��� 
	int nTotal = -1;
	//��й�ȣ
	char szPassword[100] = { 0, };
	//��й�ȣ����
	bool bPasswordSet = false;
	//���� ����
	int nOwnerNum = -1;
	//������ ���� ���
	std::set<int> setUsers;

}tRoom;


