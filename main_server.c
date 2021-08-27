// Server side implementation of UDP client-server model
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

#define PORT	 8080
#define MAXLINE 1024
#define DEBUG 1

list_node_ops user_ops={
  .print_fn=printUser,
  .destroy_fn=0
};




// Driver code
int main() {

	
	list_t users;
	init_list( &users );
	int loaded_users = loadUsers(&users, USERS_BD, &user_ops); // load user from file and set all as offline
	if (loaded_users < 0)
		handle_error("Error loading users database");
	printf("caricati in memoria %d utenti\n", loaded_users);
	print_list(&users);
	int sockfd;
	 
	
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	
	
	/**
	 * Build my connection
	 */
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// main activity loop
	struct sockaddr_in sender_addr;
	struct sockaddr_in recipient_addr;
	int ret;
	int read_bytes;
	
	socklen_t sender_addr_len = (socklen_t)sizeof(sender_addr);
	socklen_t recipient_addr_len = (socklen_t)sizeof(recipient_addr);
	Packet pack = {0};
	size_t pack_size = sizeof(Packet);
	
	
	while(1)
	{
		/* 
		All structures used to store information need to be zeroed every time loop restart to avoid disasters
		*/
		memset(&sender_addr, 0, sender_addr_len);
		memset(&recipient_addr, 0, recipient_addr_len);
		
		memset(&pack, 0, pack_size);

		read_bytes = 0;

		read_bytes = recvfrom( sockfd, &pack, PACKET_MAX_LEN, MSG_WAITALL, ( struct sockaddr *)&sender_addr, &sender_addr_len);

		if (read_bytes == pack_size && DEBUG)  {
			printf("success\n");
			print_packet(&pack);
		}

		// now i have the packet filled with the data sent from the client, is time to switch in base of type of request
		// ognuna di queste procedure  fa qualcosa, ma tutte si accumunano per spedire un dato di ritorno al client
		Type req = pack.type;
		/*
		if ( req == NEWUSER) {
			handle_new_user( sockfd, &users, &pack, ( struct sockaddr *)&sender_addr, sender_addr_len, &user_ops);
			print_list(&users);
		}
		*/
		
		switch ( req )
		{
			case NEWUSER:
				handle_new_user( sockfd, &users, &pack, ( struct sockaddr *)&sender_addr, sender_addr_len, &user_ops);
				
			case LOGIN:
				handle_login( sockfd, &users, &pack, ( struct sockaddr *)&sender_addr, sender_addr_len, &user_ops );
				
			default:
				printf("ma che vuoi?");
		}
		
	}
	return 0;
}
