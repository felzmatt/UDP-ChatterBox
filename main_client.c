// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "mylist.h"
#include "server.h"
#include "client.h"


#define MAXLINE 1024

#define MSG_DATA "Messaggio di prova\nSto cercando di contattarti tramite UDPChatterBox\n\0"



// Driver code
// Future implementation require ip address in input

// Boolean CONNECTED = FALSE;

pthread_t threads[2] = { 0 };

int main(int argc, char ** argv ) {

	printf("%s%s%s\n", CYAN, PRETTY_WELCOME, END_COLOR);

	printf("You need to login in order to use this service.\nIf don't have done yet, register!\n");

    
	int sockfd;
	int port = 8080;

	thread_args_t targs = { 0 };

	
	
	struct sockaddr_in	 servaddr;
	
	

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	socklen_t len = (socklen_t)sizeof(servaddr);
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);

	// at this point, you have to stay stuck until you successfully login

	MyInfo me = { 0 };
	char command[COMMAND_MAX_LEN] = {0};
	int ret;

	while (1) {

		while ( ! me.connected )
		{
			
			printf("Type your choice then press Enter\n[1] NEWUSER\n[2] LOGIN\n");
			printf(">> ");
			fflush(stdout);
			
			ret = get_user_input("NEWUSER or LOGIN\n>> ", command, COMMAND_MAX_LEN);
			
			if ( strncmp(command, "NEWUSER", ret) == 0) {
				ret = newuser_command( sockfd, (struct sockaddr*)&servaddr, len);
				printf("return code : %d\n", ret);

			} else if ( strncmp( command, "LOGIN", ret) == 0 ) {
				
				// printf("%sLogin is under implementation phase%s\n", RED, END_COLOR);
				ret = login_command(sockfd, &me, (struct sockaddr*) &servaddr, len);
				printf("return code %d\n", ret);
			
			} else {
				printf("Qui non dovresti starci\n");
			}
		}
		// Now starts the intersting part

		// first of all alloc the message inbox

		// MessageBox * inbox = ( MessageBox *) calloc( 1, sizeof(MessageBox));
		MessageBox inbox = { 0 };

		// make the targs point to already used structures

		targs.me = &me;
		targs.servaddr = &servaddr;
		targs.servaddr_len = len;
		targs.socket = sockfd;
		targs.inbox = &inbox;

		

		
		// starting threads
		pthread_create( &threads[INTERACTIVE_THREAD] , NULL , interactivity, &targs );
		pthread_create( &threads[RECEIVER_THREAD] , NULL , receiving, &targs );

		//printf("exiting from all\n");

		
		pthread_join( threads[INTERACTIVE_THREAD], NULL);
		pthread_join( threads[RECEIVER_THREAD], NULL);
	}
	

	close(sockfd);
	return 0;
}
