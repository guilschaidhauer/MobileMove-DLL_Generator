#include <iostream>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>

using namespace std;

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define SERVER "192.168.2.193"  //ip address of udp server

SOCKET s;
struct sockaddr_in server, si_other;
int slen, recv_len;
char buf[BUFLEN];
WSADATA wsa;

// Declare structure to be used to pass data from C++ to Mono.
struct Circle
{
	Circle(int x, int y, int radius) : X(x), Y(y), Radius(radius) {}
	int X, Y, Radius;
};

extern "C" int __declspec(dllexport) __stdcall  Init()
{
	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall  Close()
{
	closesocket(s);
	WSACleanup();
}

void parseString(string s, Circle *circle)
{
	std::string delimiter = "|";
	bool firstLoop = false;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		if (!firstLoop)
		{
			circle->X = std::stof(token, nullptr);
			firstLoop = true;
		}
		else
		{
			circle->Y = std::stof(token, nullptr);
		}
		s.erase(0, pos + delimiter.length());
	}

	circle->Radius = std::stof(s, nullptr);
}

extern "C" void __declspec(dllexport) __stdcall RunServer(Circle* outFaces)
{
	//printf("Waiting for data...");
	fflush(stdout);

	//clear the buffer by filling null, it might have previously received data
	memset(buf, '\0', BUFLEN);

	//try to receive some data, this is a blocking call
	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		//printf("recvfrom() failed with error code : %d", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}
	else
	{
		string str(buf);
		parseString(str, &outFaces[0]);
		//outFaces[0].Z = 99;
	}

	//print details of the client/peer and the data received
	//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	//printf("Data: %s\n", buf);
	//string str(buf);
	//parseString(str, &outFaces[0]);

	//now reply the client with the same data
	/*if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}*/
}