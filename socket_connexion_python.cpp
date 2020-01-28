#include "socket_connexion_python.h"

SocketConnexion::SocketConnexion(bool& error)
{
	// init winsock
	i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);  // request for version 2.2 of winsock
	if (i_result != 0) {
		std::cout << "[-] WSA startup failed : " << i_result << std::endl;
		error = true;
		return;
	}

	
	// used to declare the type of communication (TCP, IPv4, etc)
	ZeroMemory(&hints, sizeof(hints));	// set to zero the whole block of memory
	hints.ai_family = AF_INET;			// ipv4
	hints.ai_socktype = SOCK_STREAM;	// stream socket
	hints.ai_protocol = IPPROTO_TCP;	// tcp protocol
	hints.ai_flags = AI_PASSIVE;		// intend to use returned socket address struct

	// ANSI host name to address
	i_result = getaddrinfo(nullptr, PORT, &hints, &res);
	if (i_result != 0) {
		std::cout << "[-] getaddrinfo failed : " << i_result << std::endl;
		WSACleanup();
		error = true;
		return;
	}

	// Listening for new connections
	listen_socket = INVALID_SOCKET;
	listen_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// check if it is a valid address
	if (listen_socket == INVALID_SOCKET) {
		std::cout << "[-] Error creating the socket : " << WSAGetLastError() << std::endl;
		freeaddrinfo(res);  // free memory allocated by getaddrinfo
		WSACleanup();
		error = true;
		return;
	}

	// Binding the socket
	i_result = bind(listen_socket, res->ai_addr, (int)res->ai_addrlen);
	if (i_result == SOCKET_ERROR) {
		std::cout << "[-] Bind failed with error : " << WSAGetLastError() << std::endl;
		freeaddrinfo(res);
		closesocket(listen_socket);
		WSACleanup();
		error = true;
		return;
	}

	freeaddrinfo(res);

	// Listening on a socket with a reasonable number of queue length
	if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "[-] Listen failed with error : " << WSAGetLastError() << std::endl;
		closesocket(listen_socket);
		WSACleanup();
		error = true;
		return;
	}

	client_socket = INVALID_SOCKET;

	// Accept only one client
	client_socket = accept(listen_socket, nullptr, nullptr);
	if (client_socket == INVALID_SOCKET) {
		std::cout << "[-] Accept failed : " << WSAGetLastError() << std::endl;
		closesocket(listen_socket);
		WSACleanup();
		error = true;
		return;
	}

	// we no longer need server socket
	closesocket(listen_socket);

	std::cout << "[+] Server ready..." << std::endl;
	error = false;
	return;
}

SocketConnexion::~SocketConnexion()
{
	// shutdown the send half of the connection since no more data will be sent
	i_result = shutdown(client_socket, SD_SEND);
	if (i_result == SOCKET_ERROR) {
		std::cout << "[-] Shutdown of the server failed : " << WSAGetLastError() << std::endl;
		closesocket(client_socket);
		WSACleanup();
		return;
	}

	// when the other half of the response is returned we can close the connexion
	closesocket(client_socket);
	WSACleanup();
	return;
}

char* SocketConnexion::recv_data()
{
	char recv_data[BUFLEN];
	int recv_buflen = BUFLEN;

	// Receive data
	i_result = recv(client_socket, recv_data, recv_buflen, 0);
	if (i_result > 0)
		std::cout << "[*] Received : " << i_result << " bytes : " << recv_data << std::endl;

	return recv_data;
}

bool SocketConnexion::send_data(char* data)
{
	// Send an initial buffer
	i_result = send(client_socket, data, (int)strlen(data), 0);
	if (i_result == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return false;
	}

	printf("[*] Bytes Sent: %ld\n", i_result);
	return true;
}

bool SocketConnexion::send_string(std::string data)
{
	char* cdata = new char[data.length() + 1];

	// dangerous function : it can lead to remote code execution if not properly used (Buffer overflow, heap overflow, etc)
#pragma warning(disable:4996) // remove warnings because we really need this function
	std::strcpy(cdata, data.c_str());
#pragma warning(enable:4996)  // immediatly reapply warning

	bool error = this->send_data(cdata);

	delete[] cdata;

	return error;
}

