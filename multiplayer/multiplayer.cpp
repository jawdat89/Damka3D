/* ========================================================================== */
/*                                                                            */
/*   multiplayer.h                                                            */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Global functions of multiplayer                                          */
/*   are used in the main program						                      */
/* ========================================================================== */

#include <stdio.h>
#include "multiplayer.h"


SOCKET sockfde; // Variable socket for the local player

void recieveFromServer(char *buffer) {
	printf("receiving\n");
	int cc = recv(sockfde, buffer, sizeof(buffer), 0);
	if (cc == SOCKET_ERROR)
		buffer = NULL;
	printf("%s\n", buffer);
}

char* recvAll(char *buffer) {
	int bytesRead = 0;
	char c;
	char *temp = buffer;
	char *result;
	recv(sockfde, temp, 1, 0);
	c = *temp;
	while (c != '.') {
		int n = recv(sockfde, ++temp, 1, 0);
		if (n != SOCKET_ERROR && n > 0) {
			c = *temp;
			bytesRead++;
		} else
			return NULL;
	}
	result = (char*)malloc(sizeof(char)*bytesRead + 1);
	for (int i = 0; i < bytesRead; i++)
		result[i] = buffer[i];
	result[bytesRead] = '\0';
	return result;
}

void sendToServer(const char* buffer) {
	//send(sockfde, buffer, strlen(buffer), 0);
	sendAllToServer(sockfde, buffer, strlen(buffer));
}

bool sendAllToServer(int socket, const char *buffer, size_t length)
{
	char *ptr = (char*)buffer;
	while (length > 0){
		int i = send(socket, ptr, length, 0);
		if (i < 1) return false;
		ptr += i;
		length -= i;
	}
	return true;
}

int initConnection()
{
	struct sockaddr_in servaddr;
	WSADATA	wsadata;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
		return 0;

	sockfde = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfde == INVALID_SOCKET)
		return 0;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if ((servaddr.sin_addr.s_addr = inet_addr("127.0.0.1")) == INADDR_NONE)
		return 0;

	if (connect(sockfde, (SA *)&servaddr, sizeof(servaddr)) < 0)
		return 0;

	return 1;
}

void closeConnection() {
	if (sockfde)
		closesocket(sockfde);
	WSACleanup();
}