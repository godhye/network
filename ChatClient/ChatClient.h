#pragma once

#include "Network.h"
class ChatClient: public Network
{
public:
	ChatClient();
	~ChatClient();

	int Init();
	int Run();

private:
	 
};

