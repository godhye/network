// uvtest.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "uv.h"
static void on_close(uv_handle_t* handle);
static void on_connect(uv_connect_t* req, int status);
static void on_write(uv_write_t* req, int status);
uv_tcp_t pSock[10];
static uv_loop_t *loop;

 void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
	static char base[1024];

	buf->base = base;
	buf->len = sizeof(base);
}

void on_close(uv_handle_t* handle)
{
	printf("closed.\n");
}

void on_write(uv_write_t* req, int status)
{
	if (status) {
		perror("uv_write error ");
		return;
	}
	printf("wrote.\n");
	free(req);
	//uv_close((uv_handle_t*)req->handle, on_close);
}

void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)
{
	printf("on_read. %p\n", tcp);
	if (nread >= 0) {
		//printf("read: %s\n", tcp->data);
		printf("read: %s\n", buf->base);
	}
	else {
		//we got an EOF
		uv_close((uv_handle_t*)tcp, on_close);
	}

	//cargo-culted
	free(buf->base);
}

void write2(uv_stream_t* stream, const char *data, int len2) {
	uv_buf_t buffer[1];
	buffer->base =(char*)data;
	sprintf_s(buffer->base, 100 , "%s", data );
	buffer->len = len2;


	uv_write_t *req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_write(req, stream, buffer, 1, on_write);
}

int i = 0;
void on_connect(uv_connect_t* connection, int status)
{
	printf("pConn.2 %p \n", connection);
	 
	if (status < 0) {
		printf("failed to connect\n"); return;
	}

	if ((connection->data))
	{

	}
	//int n =  uv_read_start((uv_stream_t*)&pSock[i], alloc_cb, on_read);
	int n =  uv_read_start((uv_stream_t*)connection->data, alloc_cb, on_read);
	
 	if(n != 0)
		//printf("connected. %p %d  \n", connection, status );
		printf("connected error. %p %d %s \n", connection, status, uv_strerror(n));

	i++;
}
  
int main()
{
    std::cout << "Hello World!\n";

 
	struct sockaddr_in dest;
	loop = uv_default_loop();
	int i;
	for (i = 0; i < 10; i++)
	{
		uv_tcp_init(loop, &pSock[i]);
		uv_ip4_addr("118.217.182.242", 18000, &dest);

		uv_tcp_keepalive(&pSock[i], 1, 60);
		uv_connect_t *pConn = (uv_connect_t*)malloc(sizeof(uv_connect_t));
		printf("pConn.1 pConn %p %p \n", pConn->handle ,&pSock[i]);
 
		//uv_connect_t data ( non-use data) connect  pSock 
		pConn->data = (uv_stream_t*)&pSock[i];
		int n = uv_tcp_connect(pConn, &pSock[i], (const struct sockaddr*)&dest, on_connect);
		if(n != 0)
			printf("main error. %s \n", uv_strerror(n));
		
 
	}
	uv_run(loop, UV_RUN_DEFAULT);


	while (1) {
		//printf(".");
	}

}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
 
