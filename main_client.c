// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"
#include "server.h"
#include "client.h"


#define MAXLINE 1024

#define MSG_DATA "Messaggio di prova\nSto cercando di contattarti tramite UDPChatterBox\n\0"



// Driver code
// Future implementation require ip address in input

Boolean CONNECTED = FALSE;

int main(int argc, char ** argv ) {

	printf("%s%s%s\n", CYAN, PRETTY_WELCOME, END_COLOR);

	printf("You need to login in order to use this service.\nIf don't have done yet, register!\n");

    
	int sockfd;
	int port = 8080;
	
	struct sockaddr_in	 servaddr;
	
	

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	socklen_t len = sizeof(servaddr);
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	int ret = newuser_command(sockfd, &servaddr, len);
	printf("return code : %d\n", ret);


	/*
	
	int n;

	// my name and password
	char username[UNAME_MAX_LEN] = {0};
	char password[PWD_MAX_LEN] = {0};



	int username_len = strlen(argv[1]);
	int password_len = strlen(argv[2]);

	strncpy(username, argv[1], username_len);
	strncpy(password, argv[2], password_len);

	// my sender 
	char recipient[UNAME_MAX_LEN] = {0};

	int recipient_len = strlen(argv[3]);
	strncpy(recipient, "matteo23", 8);
	

	Packet pack = {0};
	
	pack.type = LOGIN;
	strncpy(pack.sender, username, username_len);
	// strncpy(pack.recipient, "gesù", 5);
	strncpy(pack.data, password, password_len);

	char buffer[MAXLINE] = {0};



	
	sendto(sockfd, &pack, sizeof(pack),
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
	printf("Login requested\n");
		
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, (struct sockaddr *) &servaddr,
				&len);
	buffer[n] = '\n';
	printf("Login success %s\n", buffer);

	memset(buffer, 0, MAXLINE);
	n = 0;
	

	// for test purposes
	if (strncmp(username, "matteo23",8) == 0)
	{
		// matteo è registrato e lo mettiamo in ascolto per prendere il messaggio
		printf("Waiting for message..\n");
		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, (struct sockaddr *) &servaddr,
				&len);
		buffer[n] = '\n';
		printf("Message arrived %s\n", buffer);
	} else {
		printf("Io sarei il branch di invio..\n");
		// brute way to stop lol xD
		int a = getchar();
		int msg_data_len = strlen(MSG_DATA);
		memset(buffer, 0, MAXLINE);

		memset(&pack, 0, sizeof(Packet));

		// build pack to send
		pack.type = MESSAGE;
		strncpy(pack.sender, username, username_len);
		strncpy(pack.recipient, recipient, recipient_len);
		strncpy(pack.data, MSG_DATA, msg_data_len);
		// packed

		sendto(sockfd, &pack, sizeof(pack),
			MSG_CONFIRM, (const struct sockaddr *) &servaddr,
				sizeof(servaddr));
		printf("Message send try\n");
		
		n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				MSG_WAITALL, (struct sockaddr *) &servaddr,
				&len);
		buffer[n] = '\n';
		printf("Message result %s\n", buffer);
	}
	*/
	
	
	



	close(sockfd);
	return 0;
}
