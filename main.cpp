#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int SERVER_PORT = 8080;

int main() {
	WSADATA wsaData;
	SOCKET serverSocket = INVALID_SOCKET;
	struct sockaddr_in serverAddr;
	int addrLen = sizeof(serverAddr);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cerr << "WinSock initialization error" << endl;
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		cerr << "Socket creation error: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(SERVER_PORT);

	if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		cerr << "Socket binding error: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Listening error: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server running on http://localhost:" << SERVER_PORT << endl;

	while (true) {
		SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&serverAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {
			cerr << "Connection error: " << WSAGetLastError() << endl;
			closesocket(serverSocket);
			WSACleanup();
			break;
		}

		char buffer[1024] = { 0 };
		recv(clientSocket, buffer, 1024, 0);
		cout << "REQUEST: " << buffer << endl;

		string jsonResponse = R"({"message": "Hello World"})";

		string httpResponse =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: " + to_string(jsonResponse.length()) + "\r\n"
			"\r\n" +
			jsonResponse;

		send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);

		closesocket(clientSocket);
	}

	closesocket(serverSocket);
	WSACleanup();
}