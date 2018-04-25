#include <iostream>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 256

using namespace std;

DWORD WINAPI ServiceClient(LPVOID lpParameters);
void PrintLocalIPAddress();

int main(void)
{
	// Initialise WinSock
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != NO_ERROR) {
		cout << "WSAStartup() failed with error:" << res << endl;
		return 1;
	}

	PrintLocalIPAddress();

	// Create socket
	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET) {
		cout << "socket function failed with error: " << WSAGetLastError() << endl;
		return 2;
	}

	// Bind socket
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.S_un.S_addr = ADDR_ANY;
	service.sin_port = htons(2345);
	memset(service.sin_zero, 0, 8);

	res = bind(server, (SOCKADDR*) &service, sizeof(service));
	if (res == SOCKET_ERROR) {
		cout << "Bind function failed with error: " << WSAGetLastError() << endl;
		return 3;
	}

	// List for connection requests
	if (listen(server, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen function failed with error: " << WSAGetLastError() << endl;
		return 4;
	}

	// Accept connection requests
	sockaddr_in clientaddress;
	int size = sizeof(clientaddress);
	while (true) {
		SOCKET client = accept(server, (SOCKADDR*) &clientaddress, &size);
		if (client == INVALID_SOCKET) {
			cout << "Accept failed with error: " << WSAGetLastError() << endl;
			return 5;
		}
		else {
			char buffer[BUFFER_SIZE];
			InetNtop(AF_INET, &clientaddress.sin_addr, buffer, BUFFER_SIZE);
			cout << "Client@IP: " << buffer << " connected." << endl;
			CreateThread(NULL, 0, ServiceClient, (LPVOID)client, 0, NULL);
			//ServiceClient(client);
		}
	}

	// Terminate server
	closesocket(server);
	WSACleanup();

	system("pause");
	return 0;
}

void PrintLocalIPAddress() {
	char buffer[BUFFER_SIZE];
	if (gethostname(buffer, BUFFER_SIZE) != SOCKET_ERROR) {
		cout << "hostname: ";
		for (int i(0); buffer[i] != '\0'; i++) {
			cout << buffer[i];
		}
		cout << endl << "IP address: ";
	}

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// hold results
	addrinfo *result = NULL;

	if (!getaddrinfo(buffer, NULL, &hints, &result) && result) {
		addrinfo* ptr = result;
		while (ptr) {
			if (ptr->ai_family == AF_INET) {
				//wchar_t wbuffer[BUFFER_SIZE];
				sockaddr_in* sockaddr_ipv4 = (sockaddr_in*) ptr->ai_addr;
				InetNtop(AF_INET, &sockaddr_ipv4->sin_addr, buffer, BUFFER_SIZE);
				//WideCharToMultiByte(CP_ACP, 0, wbuffer, -1, buffer, BUFFER_SIZE, 0, NULL);
				cout << buffer << endl;
			}
			ptr = ptr->ai_next;
		}
		freeaddrinfo(result);
	}
}


DWORD WINAPI ServiceClient(LPVOID lpParameters)
//void ServiceClient(SOCKET client)
{
	SOCKET client = (SOCKET) lpParameters;

	bool alive = true;

	char buffer[BUFFER_SIZE];
	int read = recv(client, buffer, BUFFER_SIZE, 0);

	while (read == 1 && alive) {
		if (buffer[0] == 1) {
			send(client, "Hello", 5, 0);
		}
		else if (buffer[0] == 2) {
			strcpy_s(buffer, BUFFER_SIZE, "TEST");
			send(client, buffer, strlen(buffer), 0);
		}
		else if (buffer[0] == 3) {
			strcpy_s(buffer, BUFFER_SIZE, "Give me a number to square: ");
			send(client, buffer, strlen(buffer), 0);

			read = recv(client, buffer, BUFFER_SIZE, 0);
			if (read == 1) {
				int val = buffer[0] * buffer[0];
				strcpy_s(buffer, BUFFER_SIZE, "The answer is: ");
				_itoa_s(val, buffer + strlen(buffer), 200, 10);
				send(client, buffer, strlen(buffer), 0);
			}
		}
		else if (buffer[0] == 9) {
			alive = false;
		}
		else {
			strcpy_s(buffer, BUFFER_SIZE, "Unknown request");
			send(client, buffer, strlen(buffer), 0);
		}
		read = recv(client, buffer, BUFFER_SIZE, 0);
	}

	return 0;
}