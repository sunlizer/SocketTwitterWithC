/*
 * main.c
 *
 *  Created on: 26 Kas 2014
 *      Author: sunlizer
 */
#include "main.h"

int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}

	puts("Connected\n");

	//keep communicating with server
	pthread_t sniffer_thread;
	if (pthread_create(&sniffer_thread, NULL, connection_handler, &sock)
			< 0) {
		perror("could not create thread");
		return 1;
	}

	while (1) {

		comment c = getComment();
		if (strcmp(c.command, "null") != 0) {
			if (send(sock, &c, sizeof(struct comment), 0) < 0) {
				puts("Send failed");
				return 1;
			}
		} else {
			printf("wrong entry");
		}
	}

	return 0;
}
void *connection_handler(void *socket_desc) {
	int  socket = *(int*)socket_desc;
	Message message;
	while (1) {
		if (recv(socket, &message, sizeof(Message), 0) > 0) {
			if(strcmp(message.message,"this user is already online")==0){
				printf("%s\n", message.message);
				return 0;
			}
			else if(strcmp(message.message,"start")==0){
				printf("\n----------------------------------------------------------\n");
			}
			else if(strcmp(message.message,"end")==0){
				printf("----------------------------------------------------------\n comment : ");
			}
			else
				printf("%s\n", message.message);
		}
	}
	printf("over\n");
}

comment getComment() {
	comment newComment;
	printf("Comment : ");
	scanf("%[^\n]%*c", newComment.command);
	if (strcmp(newComment.command, Commands[0]) == 0) {
		if(strcmp(user,"0")==0){
			printf("User Name : ");
			scanf("%[^\n]%*c", newComment.name);
			strcpy(user, newComment.name);
		}
		else{
			strcpy(newComment.command, "null");
			printf("you are already logged in\n");
		}
	} else if (strcmp(newComment.command, Commands[1]) == 0) {
	} else if (strcmp(newComment.command, Commands[2]) == 0) {
		printf("User Name : ");
		scanf("%[^\n]%*c", newComment.username);
	} else if (strcmp(newComment.command, Commands[3]) == 0) {
		printf("Text : ");
		scanf("%[^\n]%*c", newComment.text);
	} else if (strcmp(newComment.command, Commands[4]) == 0) {
	} else {
		strcpy(newComment.command, "null");
		printf("wrong entry\n");
	}
	return newComment;
}
