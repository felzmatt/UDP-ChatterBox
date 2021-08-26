#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"

#define UNAME_MAX_LEN 16
#define PWD_MAX_LEN 16
#define DATA_MAX_LEN 256
#define PACKET_MAX_LEN 292

#define USERS_BD "users.db"

// errori

#define EXISTS_ALREADY "UNAVAILABLE USERNAME, CHOOSE ANOTHER ONE\0"
#define CREATED_USER "SUCCESSFULLY CREATED USER\0"

#define handle_error(msg) do { fprintf(stderr,"%s\n",msg); exit(-1); }while(1);


typedef enum type {
    NEWUSER,
    LOGIN,
    MESSAGE,
    WHOSONLINE,
    DISCONNECT
} Type;

typedef enum boolean {
    FALSE = 0,
    TRUE = 1
} Boolean;

typedef struct user {
    list_node_t node;
    char username[UNAME_MAX_LEN];
    char password[PWD_MAX_LEN];
    Boolean online;
} User;

typedef struct packet {
    Type type;
    char sender[UNAME_MAX_LEN];
    char recipient[UNAME_MAX_LEN];
    char data[DATA_MAX_LEN];
} Packet;


int initUser( User * user , list_node_ops * ops);
void printUser ( list_node_t* node );
void destroyUser( User * user );
int loadUsers( list_t * users , const char * filepath , list_node_ops * ops);

User * find_user_by_username( list_t * users, const char * username );

// first approach from clients
int newUser( void * packet );
int loginUser ( void * packet );

void print_packet( Packet * pack);



int handle_new_user( int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t  sock_len, list_node_ops * ops);

