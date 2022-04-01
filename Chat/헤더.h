#pragma once

typedef struct sDATAEXP 
{
	size_t nfilesize;
	char szFilename[100];

}tDATAEXP;


/*
//����Ÿ�� 
0 //PROTOCOL_OPEN			// ä�ù氳��-���� , ��й�ȣ  
0 //PROTOCOL_JOIN			// ���� -  ��й�ȣ
0 //PROTOCOL_MSG			// ä�� ���� 
1 //PROTOCOL_UPDATE			// ä�ù����� , ��й�ȣ 
2 //PROTOCOL_FILE			// ���ϸ� , ���� ������ 
3 //PROTOCOL_QUIT			// ����

*/

//�޼�������  ����Ÿ��&�����ͻ�����&������


typedef struct sDATAJOIN
{
	int nJoinRoom; //�������ϴ� ä�ù� 
	char szPassword[BUFSIZ] = { 0, }; // ��й�ȣ 

}tDATAJOIN;

typedef struct sDATAOPEN
{
	char szPassword[BUFSIZ] = { 0, }; // ��й�ȣ 
	char szTitle[BUFSIZ];

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
#define PROTOCOL_QUIT 5 

#define BUF_SIZE 1024 // 1K