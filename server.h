#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "linked_list.h"

#define UNAME_MAX_LEN 16
#define PWD_MAX_LEN 16
#define DATA_MAX_LEN 256
#define PAYLOAD_MAX_LEN 292


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
    ListItem node;
    char username[UNAME_MAX_LEN];
    char password[PWD_MAX_LEN];
    Boolean connected;
} User;

typedef struct packet {
    Type type;
    char sender[UNAME_MAX_LEN];
    char recipient[UNAME_MAX_LEN];
    char data[DATA_MAX_LEN];
} Packet;


int initUser( User * user , ListItemOps * ops);
void printUser ( User * user );
void destroyUser( User * user );
int loadUsers( ListHead * users , const char * filepath );

// first approach from clients
int newUser( void * packet );
int loginUser ( void * packet );


















// loop of server
void activity_loop ();

