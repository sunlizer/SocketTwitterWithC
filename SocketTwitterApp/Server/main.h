/*
 * main.h
 *
 *  Created on: 1 Ara 2014
 *      Author: sunlizer
 */

#ifndef MAIN_H_
#define MAIN_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define PORT 1453

typedef struct comment comment;
typedef struct Message Message;


typedef struct user user;
int lastUser = 1;

char *follow(int follower, int userToFollow);
int isFollow(int follower, int userToFollow);
void *connection_handler(void *socket_desc);
int findUser(char name[10]);
int findUserWithSocket(int id);

//#define struct comment Comment
struct Message {
	char sendedTo[10];
	char message[1024];
};

struct comment {
	char user[10];
	char command[10];
	char username[10];
	char text[1024];
};
struct user {
	char name[10];
	int isOnline;
	int following[100];
	int lastEntry;
	int socket;
};
void start(Message msg,int sock){
	strcpy(msg.message, "start");
	if (send(sock, &msg, sizeof(Message), 0) < 0) {
		puts("Send failed");
	}
}
void end(Message msg, int sock){
	strcpy(msg.message, "end");
	if (send(sock, &msg, sizeof(Message), 0) < 0) {
		puts("Send failed");
	}
}
char Commands[5][10] = { "login", "getList", "follow", "message","getflw" };
struct user users[100];


#endif /* MAIN_H_ */
