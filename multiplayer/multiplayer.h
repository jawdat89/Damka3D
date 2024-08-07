/* ========================================================================== */
/*                                                                            */
/*   multiplayer.h                                                            */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Global functions declaration of multiplayer                              */
/*   are used in the main program						                      */
/* ========================================================================== */

#pragma once
#include <winsock2.h> // Holds all the functions needed for sockets

#define SA struct sockaddr // For easier coding
#define MAXLINE 128 // Maximum chars in sent and recieved messages 
#define SERV_PORT 54433 // Server port

void recieveFromServer(char* buffer); // A function used to listen to server for messages and stores it in the buffer
int initConnection(); // Initialize connection with the server
void closeConnection(); // Close the connection
void sendToServer(const char *buffer); // A function used to send a message to the server
bool sendAllToServer(int socket, const char *buffer, size_t length); // A function used to send a message to the server one char at a time until it sends everything (for maximum reliability)
char* recvAll(char *buffer); // A function used to listen to server for messages and accepts the message one char at a time until there's no more