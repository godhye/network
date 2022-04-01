#pragma once

typedef struct sDATAEXP 
{
	size_t nfilesize;
	char szFilename[100];

}tDATAEXP;


/*
//서비스타입 
0 //PROTOCOL_OPEN			// 채팅방개설-제목 , 비밀번호  
0 //PROTOCOL_JOIN			// 입장 -  비밀번호
0 //PROTOCOL_MSG			// 채팅 전송 
1 //PROTOCOL_UPDATE			// 채팅방제목 , 비밀번호 
2 //PROTOCOL_FILE			// 파일명 , 파일 사이즈 
3 //PROTOCOL_QUIT			// 퇴장

*/

//메세지형식  서비스타입&데이터사이즈&데이터


typedef struct sDATAJOIN
{
	int nJoinRoom; //참여원하는 채팅방 
	char szPassword[BUFSIZ] = { 0, }; // 비밀번호 

}tDATAJOIN;

typedef struct sDATAOPEN
{
	char szPassword[BUFSIZ] = { 0, }; // 비밀번호 
	char szTitle[BUFSIZ];

}tDATAOPEN;

typedef struct sDATAMSG
{
	int nJoinRoom; //참여원하는 채팅방 
	char szPassword[BUFSIZ] = { 0, }; // 비밀번호 

}tDATAMSG;

#define PROTOCOL_OPEN 0 
#define PROTOCOL_JOIN 1
#define PROTOCOL_MSG 2
#define PROTOCOL_UPDATE 3 
#define PROTOCOL_FILE 4
#define PROTOCOL_QUIT 5 

#define BUF_SIZE 1024 // 1K