#pragma once
#include <winsock2.h>
#include <string>
class SockServer
{
	bool m_serverOn;
	bool m_clientOn;
	bool m_accepted;
	static const int CommandsNumber = 3;
	std::string Commands[CommandsNumber];
	std::string error_msg;
	SOCKET m_servSock;
	SOCKET sock;
	int SockServer::CloseServer(void);
public:
	SockServer(void);
	~SockServer(void);
	void Start(void);
	void Work(void);
	void ShutDown(void);
private:
	int Initialize(void);
	int Listen(void);
	int AcceptClient(SOCKET &);
	int HandleClient(SOCKET & client);
	bool SockServer::RecognizeCommand(const char * rcvdmessage, int & commandIndex, std::string & key, std::string & value);
	bool ExecuteCommand(int commIndex, std::string key, std::string&);
public:
	void Bind(void);
};

