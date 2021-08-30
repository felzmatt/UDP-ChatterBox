#pragma once

#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "mylist.h"
#include "server.h"

#define MAX_PENDINGS 50 

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

#define COMMAND_MAX_LEN 20

typedef struct message_box_s {
    
    int size;
    int last_read;
    Packet  message_buffer[MAX_PENDINGS];

} MessageBox;

typedef struct myinfo {
    char username[UNAME_MAX_LEN];
    char password[PWD_MAX_LEN];
    Boolean connected;
} MyInfo;

typedef struct thread_args_s {
    
    // my info
    MyInfo * me;
    // connection details
    int socket;
    struct sockaddr_in * servaddr;
    socklen_t servaddr_len;
    // message inbox
    MessageBox * inbox;
    sem_t semaphore;

} thread_args_t;



int get_user_input(const char * label, char * input, int maxlen);


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
int login_command( int socket, MyInfo * me,  struct sockaddr * servaddr, socklen_t servaddr_len );

void interactivity( void * args );

int pm_command(int socket, MyInfo * me, struct sockaddr * servaddr, socklen_t servaddr_len, sem_t * semaphore);
int spm_command( MessageBox * inbox );
int logout_command( int socket, MyInfo * me, struct sockaddr * servaddr, socklen_t servaddr_len);
void receiving( void * args );