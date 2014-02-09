#include "SockServer.h"
#include "XMLWorker.h"
#include <stdio.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
SockServer::SockServer(void)
{
	m_accepted = m_clientOn = m_serverOn = false;
	Commands[0] = "add";
	Commands[1] = "get";
	Commands[2] = "set";
}


SockServer::~SockServer(void)
{
}

 
int SockServer::CloseServer(void)
{
    //Close server socket
    closesocket(m_servSock);
    WSACleanup();
    return 0;
}

void SockServer::Start(void)
{
	if(Initialize()) //Initialize
		return;
	m_serverOn = true;

}

void SockServer::Work(void)
{
	if (m_accepted && m_serverOn) //HandleClient
	{
		if(HandleClient(sock))
			m_accepted = false;
	}
}
void SockServer::ShutDown(void)
{
	if(m_clientOn)
	{
		closesocket(sock);
		CloseServer();
	}
	else
	{
		if(m_serverOn)
			CloseServer();
	}
	m_clientOn = m_serverOn = false;
}


int SockServer::Initialize(void)
{
	WORD sockVer;
    WSADATA wsaData;
    sockVer = MAKEWORD(2,2); // socket version
 
    WSAStartup(sockVer, &wsaData);
 
    m_servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // create socket
 
    if(m_servSock == INVALID_SOCKET)
    {
        //printf("Unable to create socket\n");
        WSACleanup();
        return SOCKET_ERROR;
    }
	return 0;
}


int SockServer::Listen(void)
{
	SOCKADDR_IN sin;
    sin.sin_family = PF_INET;
    sin.sin_port = htons(1111);
    sin.sin_addr.s_addr = INADDR_ANY;
	int retVal = bind(m_servSock, (LPSOCKADDR)&sin, sizeof(sin));
    if(retVal == SOCKET_ERROR)
    {
        //printf("Unable to bind\n");
        WSACleanup();
        return SOCKET_ERROR;
    }

	// start to listen socket
    retVal = listen(m_servSock, 10);
    if(retVal == SOCKET_ERROR)
    {
        //printf("Unable to listen\n");
        WSACleanup();
        return SOCKET_ERROR;
    }
	return 0;
}


int SockServer::AcceptClient(SOCKET& clientSock)
{
	//waiting for a client
	timeval acceptTimeOut;
	acceptTimeOut.tv_sec = 1L;
	acceptTimeOut.tv_usec = 0L;
	fd_set read_s; // check Read possibility
	FD_ZERO (&read_s); // set to empty
	FD_SET (m_servSock, &read_s); // add client socket
    int selectResult = select(0, &read_s, NULL, NULL, &acceptTimeOut);
	if (selectResult == 0 || selectResult == SOCKET_ERROR) 
		return SOCKET_ERROR;
	// now it's non blocking
	clientSock = accept(m_servSock, NULL, NULL); 
    if(clientSock == INVALID_SOCKET)
    {
        //printf("Unable to accept\n");
        WSACleanup();
        return SOCKET_ERROR;
    }
	return 0;
}

// Performing send & receive operations
int SockServer::HandleClient(SOCKET & clientSock)
{
	const int ReceiveBufferSize = 255;
	char reciveBuffer[ReceiveBufferSize]; 
    int retVal = recv(clientSock, reciveBuffer, ReceiveBufferSize, 0);
	if (retVal == 0) // client closed connection
	{
		return 1;
	}
    if(retVal == SOCKET_ERROR)
    {
        //printf("Unable to recv\n");
        return SOCKET_ERROR;
    }
    //printf("Got the request from client\n%s\n",reciveBuffer);
	std::string arg1, arg2;
	int command;
	char result = static_cast<char>(RecognizeCommand(reciveBuffer, command, arg1, arg2));
	if (result)
	{
		result = static_cast<char>(ExecuteCommand(command, arg1, arg2));
	}
    //printf("Sending response from server\n");
	const char *szResp;
	error_msg.insert(0, 1, ++result);
	szResp = error_msg.c_str();
	--result;
	if(!result)
	{
		++result;
		//printf("Error text: %s\n", szResp);
		retVal = send(clientSock, szResp, strlen(szResp)+1, 0);
	}
	else
	{
		++result;
		if(command == 1) // get
		{
			arg2.insert(0, 1, result);
			szResp = arg2.c_str();
			retVal = send(clientSock, szResp, strlen(szResp)+1, 0);
		}
		else
		{
			retVal = send(clientSock, szResp, strlen(szResp)+1, 0);
		}
	}
	error_msg.clear();
    if(retVal == SOCKET_ERROR)
    {
        //printf("Unable to send\n");
        return SOCKET_ERROR;
    }
	return 0;
}

bool SockServer::RecognizeCommand(const char * rcvdmessage, int & commandIndex, std::string & key, std::string & value)
{
	std::string message(rcvdmessage);
	// trin from start
	std::string  & s = message;
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	std::string firstArg;
	firstArg.assign(s.substr(0, s.find(' ')));

	int commIndex = 0;
	s.erase(0, firstArg.length());
	std::transform(firstArg.begin(), firstArg.end(), firstArg.begin(), std::tolower );
	for(;commIndex < CommandsNumber; commIndex++)
	{
		if(!firstArg.compare(Commands[commIndex]))
			break;
	}
	if(commIndex >= CommandsNumber)
	{
		error_msg = "Command not recognized";
		return false; // command not recognized
	}
	commandIndex = commIndex;
	// find the first argument
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	firstArg.clear();
	firstArg.assign(s.substr(0, s.find(' ')));
	key = firstArg;
	// find the second argument if any
	s.erase(0, firstArg.length());
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace)))); // left trim
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end()); // fight trim
	value = s;
	return true;
}


bool SockServer::ExecuteCommand(int commIndex, std::string key, std::string& value)
{
	XMLWorker * xmlWoker = &XMLWorker::GetInstance();
	switch(commIndex)
	{
	case 0: // add
		if(value.empty())
		{
			error_msg = "Value should not be empty";
			return false;
		}
		if(!xmlWoker->Add(key, value))
		{
			error_msg = "Key isn't unique, if you want to change value then use SET";
			return false;
		}
		xmlWoker->SaveFile(nullptr);
		break;
	case 1: // get
		if(!xmlWoker->Get(key, value))
		{
			error_msg = "Key not found";
			return false;
		}
		break;
	case 2: // set
		if(value.empty())
		{
			error_msg = "Value should not be empty";
			return false;
		}
		if(!xmlWoker->Set(key, value))
		{
			error_msg = "Key not found, if you want to add it then use ADD";
			return false;
		}
		xmlWoker->SaveFile(nullptr);
		break;
	default:
		return false;
	}
	return true;
}


void SockServer::Bind(void)
{
	if(!m_serverOn)
		return;
	if(m_accepted)
		return;
	if(m_clientOn || !Listen()) // Listen
	{
		m_clientOn = true;
		if(!AcceptClient(sock)) // AcceptClient
		{
			m_accepted = true;
			return;
		}
	}
	m_accepted = false;
}
