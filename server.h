#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "mylist.h"

#define UNAME_MAX_LEN 16
#define PWD_MAX_LEN 16
#define DATA_MAX_LEN 256
#define PACKET_MAX_LEN 292

#define USERS_BD "users.db"

// errori

// HANDLE NEWUSER
#define EXISTS_ALREADY "UNAVAILABLE USERNAME, CHOOSE ANOTHER ONE\0"
#define CREATED_USER "SUCCESSFULLY CREATED USER\0"
// HANDLE LOGIN
#define NOT_EXISTS_USER "THIS USERNAME IS NOT IN DATABASE, REGISTER OR TRY WITH ANOTHER ONE\0"
#define LOGGED_USER "SUCCESSFULLY LOGGED IN\0"
#define WRONG_PASSWORD "WRONG PASSWORD\0"
// HANDLE MESSAGE
#define USER_NOT_FOUND "THIS USER IS NOT REGISTERED IN DATABASE\0"
#define USER_NOT_ONLINE "THIS USER IS OFFLINE\0"
#define SENT_MESSAGE "CORRECTLY SENT MESSAGE\0"

// HANDLE DISCONNECT
#define DISCONNECTED_USER "YOU ARE DISCONNECTED\0"




typedef enum type {
    NEWUSER,
    LOGIN,
    MESSAGE,
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
    struct sockaddr client_address;
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



void handle_new_user( int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t  sock_len, list_node_ops * ops);
void handle_login(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t sock_len, list_node_ops * ops);
void handle_message(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t sock_len, list_node_ops * ops);
void handle_disconnect(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t sock_len, list_node_ops * ops);

