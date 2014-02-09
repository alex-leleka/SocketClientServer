#include "SocketClient.h"
#include <windows.h>
#include <iostream>

#include <algorithm> // std::transform
//#include <functional> 
#include <cctype> // std::tolower
SocketClient::SocketClient(void)
{
}


SocketClient::~SocketClient(void)
{
}


int SocketClient::Start(void)
{
	LPHOSTENT hostEnt;
	if(Initialize(hostEnt))
		return 0;
	SOCKET clientSock;
	if(CreateSocket(clientSock))
		return 0;
	if(Connect(clientSock, hostEnt))
		return 0;
	if(SendReceive(clientSock))
		return 0;
    return CloseSocket(clientSock);
}


int SocketClient::Initialize(LPHOSTENT& hostEnt)
{
	WORD ver = MAKEWORD(2,2); // protocol version
    WSADATA wsaData;
    int retVal=0;
 
    WSAStartup(ver,(LPWSADATA)&wsaData);
 
    /// Collect information of the server
    hostEnt = gethostbyname("localhost");
 
    if(!hostEnt)
    {
        std::cout << "[Client Application][ERROR]Unable to collect gethostbyname()" << std::endl;
        WSACleanup();
        return 1;
    }
	return 0;
}


int SocketClient::CreateSocket(SOCKET& clientSock)
{
	/// Creating a socket
    clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 
    if(clientSock == SOCKET_ERROR)
    {
        std::cout << "[Client Application][ERROR]Unable to create socket" << std::endl;
        WSACleanup();
        return 1;
    }
	return 0;
}


int SocketClient::Connect(SOCKET& clientSock, LPHOSTENT& hostEnt)
{
	SOCKADDR_IN serverInfo;
    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr = *((LPIN_ADDR)*hostEnt->h_addr_list);
    serverInfo.sin_port = htons(1111);
 
    int retVal=connect(clientSock,(LPSOCKADDR)&serverInfo, sizeof(serverInfo));
    if(retVal==SOCKET_ERROR)
    {
        std::cout << "[Client Application][ERROR] Unable to connect()" << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "[Client Application][SUCCESS] Connection made sucessfully" << std::endl;
	return 0;
}


int SocketClient::SendReceive(SOCKET& clientSock)
{
	const int ResultStateCount = 2; 
	const std::string ResultStateFailSuccess[ResultStateCount] = {"Fail","Success"};
	std::string requestMsg;
	while(!ConsoleReadMessage(requestMsg))
	{
		const char *pBuf = requestMsg.c_str();
		std::cout << "[Client Application][SENDING...] Sending request from client" << std::endl;
		int retVal = send(clientSock, pBuf, strlen(pBuf)+1, 0);
 
		if(retVal == SOCKET_ERROR)
		{
			std::cout << "[Client Application][ERROR] Unable to send()" << std::endl;
			WSACleanup();
			return 1;
		}
 
		const int ReceiveBufferSize = 255;
		char szResponse[ReceiveBufferSize];
		retVal = recv(clientSock, szResponse, ReceiveBufferSize, 0);
 
		if(retVal == SOCKET_ERROR)
		{
			std::cout << "[Client Application][ERROR] Unable to recv()" << std::endl;
			WSACleanup();
			return 1;
		}
		if(retVal == SOCKET_ERROR)
		{
			std::cout << "[Client Application][ERROR] Server disconnected" << std::endl;
			WSACleanup();
			return 0;
		}
		int requestResult = static_cast<int>(szResponse[0]);
		if (requestResult != 1 && requestResult != 2) // wrong incoming message format
			return 1;
		std::cout << "[Client Application] Command execution " << ResultStateFailSuccess[--requestResult] << std::endl;
		if(szResponse[1])
			std::cout << "Got the response from server: " << &szResponse[1] << std::endl;
	}
	return 0;
}


int SocketClient::CloseSocket(SOCKET& clientSock)
{
	closesocket(clientSock);
    WSACleanup();
	return 0;
}


int SocketClient::ConsoleReadMessage(std::string& userCommand)
{
	const std::string Help = "help";
	const std::string Exit = "exit";
	bool msgRead;
	do
	{
		msgRead = true;
		std::cout << "> " ;
		std::getline(std::cin, userCommand);
		if(userCommand.empty())
		{
			msgRead = false;
			continue;
		}
		std::string temp = userCommand;
		std::transform(temp.begin(), temp.end(), temp.begin(), std::tolower );
		if(temp.compare(Exit) == 0)
			return 10;
		if(temp.compare(Help) == 0)
		{
			msgRead = false;
			ConsolePrintHelp();
		}
	}
	while (!msgRead);
	return 0;
}
void SocketClient::ConsolePrintHelp(void)
{
	std::cout << "Type 'exit' for exit." << std::endl;
	std::cout << "You can use next commands: " << std::endl <<
		"	add <tag_name> <tag_value>" << std::endl << 
		"	get <tag_name>" << std::endl << 
		"	set <tag_name> <tag_value>" << std::endl;
	std::cout << "Type 'help' to see this message again." << std::endl;
}