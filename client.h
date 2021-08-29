#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"
#include "server.h"

#define RECEIVER_THREAD 0
#define INTERACTIVE_THREAD 1

#define BLACK  "\033[0;30m\0"
#define RED  "\033[0;31m\0"
#define GREEN   "\033[0;32m\0"
#define YELLOW   "\033[0;33m\0"
#define BLUE   "\033[0;34m\0"
#define MAGENTA   "\033[0;35m\0"
#define CYAN   "\033[0;36m\0"
#define WHITE  "\033[0;37m\0"
#define END_COLOR "\33[0m"

#define PRETTY_WELCOME  "Client process to interact with UDP-ChatterBox\0"

typedef struct myinfo {
    char username[UNAME_MAX_LEN];
    char password[PWD_MAX_LEN];
} MyInfo;


/**
 * ------------- RETURN CODES -------------------
 */

// NEWUSER

#define LOGIN_SUCCESS
#define LOGIN_FAIL

/**
 * ----------- COMMAND INDEXING --------------------
 */

// NOT LOGGED

#define NEWUSER_COMMAND 0
#define LOGIN_COMMAND 1

// LOGGED

/**
 * ---------------------------------------------------
 */


/**
 * ----------- COMMANDS LABELS ----------------------
 */

// NOT LOGGED

#define NEWUSER_LABEL "[0] NEWUSER\0"
#define LOGIN_LABEL "[1] LOGIN\0"


// LOGGED
#define MESSAGE_LABEL "[2] MESSAGE\0"


/**
 * --------------------------------------------------
 */


/**
 * ------------ COMMANDS PROCEDURES ------------------
 */

// NOT LOGGED

int newuser_command( int socket, struct sockaddr * servaddr, socklen_t servaddr_len );