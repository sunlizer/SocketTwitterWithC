#include "main.h"

int main() {

	int socket_desc, new_socket, c, *new_sock;
	struct sockaddr_in server, client;

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( PORT);

	//Bind
	if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
		puts("bind failed");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while ((new_socket = accept(socket_desc, (struct sockaddr *) &client,
			(socklen_t*) &c))) {
		puts("new user connected");

		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = new_socket;

		if (pthread_create(&sniffer_thread, NULL, connection_handler,
				(void*) new_sock) < 0) {
			perror("could not create thread");
			return 1;
		}

		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
	}

	if (new_socket < 0) {
		perror("accept failed");
		return 1;
	}

	return 0;
}
void *connection_handler(void *socket_desc) {
	//Get the socket descriptor
	int sock = *(int*) socket_desc;
	struct comment c;

	//wait command from user
	while (recv(sock, &c, sizeof(struct comment),  MSG_WAITALL) > 0) {
		Message msg;
		strcpy(msg.sendedTo, c.user);
		//Login
		if (strcmp(c.command, Commands[0]) == 0) {
			int find = 0;
			int i = 0;
			for (i = 1; i < lastUser; ++i) {
				if (strcmp(c.user, users[i].name) == 0) {
					if (users[i].isOnline == 1) {
						strcpy(msg.message, "this user is already online");
						if (send(sock, &msg, sizeof(Message), 0) < 0) {
							puts("Send failed");
						}
						return 0;
					} else {
						start(msg,sock);
						strcpy(msg.message, "welcome back");
						if (send(sock, &msg, sizeof(Message), 0) < 0) {
							puts("Send failed");
						}
						end(msg,sock);
					}
					users[i].isOnline=1;
					users[i].socket = sock;
					find = 1;
					break;
				}
			}
			if (find == 0) {
				start(msg,sock);
				strcpy(msg.message, "welcome :)");
				if (send(sock, &msg, sizeof(Message), 0) < 0) {
					puts("Send failed");
				}
				end(msg,sock);
				printf("new client added  : %s\n", c.user);
				user newUser;
				newUser.isOnline = 1;
				strcpy(newUser.name, c.user);
				newUser.lastEntry = 0;
				newUser.socket = sock;
				users[lastUser] = newUser;
				lastUser++;

			}
		}
		//GetList
		else if (strcmp(c.command, Commands[1]) == 0) {
			int i;
			printf("list\n");
			start(msg,sock);
			for (i = 1; i < lastUser; ++i) {
				if (users[i].isOnline == 1) {
					strcpy(msg.message, users[i].name);
					printf("%s\n", msg.message);
					if (send(sock, &msg, sizeof(Message), 0) < 0) {
						puts("Send failed");
					}

				}
			}
			end(msg,sock);
		}
		//Follow
		else if (strcmp(c.command, Commands[2]) == 0) {
			int userID = findUser(c.user);
			int flw = findUser(c.username);
			if (flw != 0) {
				strcpy(msg.message, follow(userID, flw));
				printf("%s is starting to following %s\n", c.user, c.username);

			} else {
				strcpy(msg.message, "there is no such a user");
			}
			start(msg,sock);
			if (send(sock, &msg, sizeof(Message), 0) < 0) {
				puts("Send failed");
			}
			end(msg,sock);
		} //Twit
		else if (strcmp(c.command, Commands[3]) == 0) {
			printf("new deduKodu \n");
			int userID = findUser(c.user);
			printf("UserId: %d  User Name: %s Message: %s \n", userID, c.user,
					c.text);
			int i = 1;
			sprintf(msg.message,"%s said \"%s\"",c.user, c.text);
			for (i = 1; i < lastUser; ++i) {
				if (isFollow(i, userID)) {
					start(msg,sock);
					if (send(users[i].socket, &msg, sizeof(Message), 0) < 0) {
						puts("Send failed");
					}
					end(msg,sock);
				}
			}
		}//get follower list
		else if (strcmp(c.command, Commands[4]) == 0) {
			int userID = findUser(c.user);
			int i = 1;
			start(msg,sock);
			for (i = 1; i < lastUser; ++i) {
				if (isFollow(i, userID)) {
					strcpy(msg.message,users[i].name);
					if (send(sock, &msg, sizeof(Message), 0) < 0) {
						puts("Send failed");
					}

				}
			}
			end(msg,sock);
		}
	}
	int usid=findUserWithSocket(sock);
	printf("%s disconnected\n",users[usid].name);
	users[usid].isOnline=0;

	//Free the socket pointer
	free(socket_desc);

	return 0;
}
char *follow(int follower, int userToFollow) {
	if (isFollow(follower, userToFollow)) {
		return "this user has followed before !!";
	} else {
		users[follower].following[users[follower].lastEntry] = userToFollow;
		users[follower].lastEntry++;
		return "User added to your following list";
	}
}
int isFollow(int follower, int userToFollow) {
	int i = 0;
	for (i = 0; i < users[follower].lastEntry; ++i) {
		if (users[follower].following[i] == userToFollow) {
			return 1;
		}
	}
	return 0;
}
int findUser(char name[10]) {
	int i;
	for (i = 1; i < lastUser; ++i) {
		if (strcmp(users[i].name, name) == 0) {
			return i;
		}
	}
	return 0;
}
int findUserWithSocket(int id){
	int i;
		for (i = 1; i < lastUser; ++i) {
			if (users[i].socket==id) {
				return i;
			}
		}
		return 0;
}
