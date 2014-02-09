#pragma once
#include <winsock.h>
#include <string>
class SocketClient
{
public:
	SocketClient(void);
	~SocketClient(void);
	int Start(void);
private:
	int Initialize(LPHOSTENT& hostEnt);
	int CreateSocket(SOCKET& clientSock);
	int Connect(SOCKET& clientSock, LPHOSTENT& hostEnt);
	int SendReceive(SOCKET& clientSock);
	int CloseSocket(SOCKET& clientSock);
	int ConsoleReadMessage(std::string& userCommand);
	void ConsolePrintHelp(void);
};

