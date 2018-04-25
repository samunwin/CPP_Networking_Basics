#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 256

using namespace std;

int main(void)
{
	// Initialise WinSock
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != NO_ERROR) {
		cout << "WSAStartup() failed with error: " << res << endl;
		return 1;
	}

	// Create socket
	SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connection == INVALID_SOCKET) {
		cout << "socket function failed with error: " << WSAGetLastError() << endl;
		return 2;
	}

	// Connect socket
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	InetPton(AF_INET, "127.0.0.1", &sockaddr.sin_addr);
	sockaddr.sin_port = htons(2345);


	// Connect to a server
	if (connect(connection, (SOCKADDR*) &sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		cout << "Connection function failed with error: " << WSAGetLastError() << endl;
		return -1;
	}

	// Send data
	int command = 0;
	cout << "Enter command: ";
	cin >> command;
	while (command != -1) {
		if (send(connection, (char*) &command, 1, 0) == SOCKET_ERROR) {
			cout << "Send function failed with error: " << WSAGetLastError() << endl;
			return -2;
		}

		// Receive data
		char buffer[BUFFER_SIZE];
		int read = recv(connection, buffer, BUFFER_SIZE, 0);
		if (read == SOCKET_ERROR) {
			cout << "recv function failed with serror: " << WSAGetLastError() << endl;
			return -3;
		}

		for (int i(0); i < read; i++) {
			cout << buffer[i];
		}
		cout << endl;
		//cout << "Enter command: ";
		cin >> command;
	}


	// Disconnect the socket
	closesocket(connection);
	WSACleanup();

	system("pause");
	return 0;
}
