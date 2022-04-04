#pragma once

typedef struct sDATAEXP 
{
	size_t nfilesize;
	char szFilename[100];

}tDATAEXP;

/* 패킷 형식 

서비스타입&데이터사이즈&데이터
04%d&06d&%s

*/


#define PROTOCOL_SERVIEC_TYPE 4
#define PROTOCOL_SEPARATOR 1
#define PROTOCOL_MSG_SIZE 6


/*
//서비스타입 
0 //PROTOCOL_OPEN			// 채팅방개설-제목 , 비밀번호  
0 //PROTOCOL_JOIN			// 입장 -  비밀번호
0 //PROTOCOL_MSG			// 채팅 전송 
1 //PROTOCOL_UPDATE			// 채팅방제목 , 비밀번호 
2 //PROTOCOL_FILE			// 파일명 , 파일 사이즈 
3 //PROTOCOL_QUIT			// 퇴장

*/

//메세지형식  


typedef struct sDATAJOIN
{
	int nJoinRoom; //참여원하는 채팅방 
	char szPassword[BUFSIZ] = { 0, }; // 비밀번호 

}tDATAJOIN;

typedef struct sDATAOPEN
{
	char szPassword[100] = { 0, }; // 비밀번호 
	char szTitle[100];
	int ntest = 3;
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
#define PROTOCOL_KEEPALIVE 5
#define PROTOCOL_QUIT 6

#define BUF_SIZE 1024 // 1K



typedef struct sUser {

	//소켓
	unsigned int  h_socket;
	//닉네임
	std::string strNick;
	//참가한방넘버
	int nRoomNum = -1;
} tUser;

typedef struct sRoom {
	//방번호
	int nRoomNum = -1;
	//제목
	char szTitle[100] = { 0, };
	//인원수 
	int nTotal = -1;
	//비밀번호
	char szPassword[100] = { 0, };
	//비밀번호여부
	bool bPasswordSet = false;
	//방장 정보
	int nOwnerNum = -1;
	//참여한 유저 목록
	std::set<int> setUsers;

}tRoom;


