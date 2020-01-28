// chess_engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <csignal>
#include "socket_connexion_python.h"
#include "playingModes.h"

#include "ai.h"
#include "saving.h"

void shutdownServer(int signum);

using namespace std;

int main()
{	
	// register signal SIGINT and signal handler  
	signal(SIGINT, shutdownServer);

	while (true) {
		// Establishing connection
		cout << "[+] Opening port " << PORT << "." << endl;
		bool error = false;
		SocketConnexion conn = SocketConnexion(error);
		if (error) {
			cout << "[-] There was a problem establishing the connection, leaving..." << endl;
			return 1;
		}

		bool playing = true;
		while (playing) {
			cout << "*** MENU ***" << endl;
			string mode = string(conn.recv_data());
			if (mode == "HA") {
				playingAgainstAi(conn);
			}
			else if (mode == "AA") {
				AIvsAI();
			}

			// next action
			cout << "[*] Waiting for next action." << endl;
			string action = string(conn.recv_data());
			if (action[0] == 'E' && action[1] == 'N') {
				cout << "[*] Client shutting down." << endl;
				playing = false;
			}
			else if (action[0] == 'S' && action[1] == 'T')
				continue;
		}

		cout << "[*] Reopening port in a moment for maybe a new connection later." << endl;
		Sleep(2000);  // wait 2 seconds before reopening the server
	}
	
	return 0;
}

void shutdownServer(int signum) {
	cout << "[*] Shutdown" << endl;
}