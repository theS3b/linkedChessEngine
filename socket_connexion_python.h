#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")
#define PORT		"62358"
#define BUFLEN		1024

// A Microsoft tutorial has been followed to write most of this code
// so there may be some similarities
// the tutorial :
// https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock

class SocketConnexion {
public:
	SocketConnexion(bool & error);
	~SocketConnexion();

	char* recv_data();
	bool send_data(char* data);
	bool send_string(std::string data);

private:
	WSADATA wsa_data;
	int i_result;

	// addrinfo from Ws2def.h
	struct addrinfo  hints;
	struct addrinfo  *res;
	SOCKET listen_socket;
	SOCKET client_socket;
};