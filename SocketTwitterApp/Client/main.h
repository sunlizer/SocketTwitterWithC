/*
 * main.h
 *
 *  Created on: 1 Ara 2014
 *      Author: sunlizer
 */

#ifndef MAIN_H_
#define MAIN_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<pthread.h>
#define PORT 1453

char Commands[5][10] = { "login", "getList", "follow", "message","getflw" };

typedef struct comment comment;
typedef struct Message Message;

char user[10]="0";
void *connection_handler(void *sock);

struct Message {
	char sendedTo[10];
	char message[1024];
};

struct comment {
	char name[10];
	char command[10];
	char username[10];
	char text[1024];
};
comment getComment();


#endif /* MAIN_H_ */
