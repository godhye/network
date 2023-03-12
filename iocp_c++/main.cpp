#include "Netengine.h"
//void *
#define CENTER 1
#define GW 1

int main(int argc, char* argv[])
{
 
	Netengine p; 
	p.InitNet();
	p.m_pServSock->SetIpPort((char*)"127.0.0.1", 8888);
	p.MakeClientSocket(CENTER, (char*)"127.0.0.1", 3333);
	p.MakeClientSocket(GW, (char*)"127.0.0.1", 3334);
	 
	p.Run();
 
	return 0;
}
 
void ErrorHandling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}