#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "mylist.h"
#include "server.h"
#include "client.h"

int get_user_input(const char * label, char * input, int maxlen)
{
    int len;
    printf("%s", label);
    fflush(stdout);
    fgets(input, maxlen , stdin);
    len = strlen(input);
    input[len - 1] = '\0';   // delete \n byte
    return len;
}

int newuser_command(int socket, struct sockaddr * servaddr, socklen_t servaddr_len)
{
    /**
     * This function is invoked when a not logged user prompt 1 in the terminal
     * When the user is logged this function is no longer available
     * This is the only function that can set CONNECTED = TRUE
     * 
     * This function MUST return an integer indicating the result of requested operation
     */

    // ask user for his username and his password

    size_t pack_len = sizeof(Packet);
    char username[UNAME_MAX_LEN] = {0};
    char password[PWD_MAX_LEN] = {0};

    printf("[1] %sNEWUSER PROCEDURE%s\n", CYAN,END_COLOR);
    printf("USERNAME 16 CHAR: >> ");
    fflush(stdout);
    fgets(username, UNAME_MAX_LEN, stdin);
    
    
    
    printf("PASSWORD 16 CHAR: >> ");
    fflush(stdout);
    fgets(password, PWD_MAX_LEN, stdin);

    int uname_len = strlen(username);
    int pwd_len = strlen(password);

    username[uname_len - 1] = '\0';
    password[pwd_len - 1] = '\0';

    Packet pack = { 0 };
    
    pack.type = NEWUSER;
    strncpy( pack.sender , username, uname_len);
    strncpy( pack.data, password, pwd_len);

    int written_bytes = 0;
    int read_bytes = 0;

    char response[1024] = {0};

    do {
        
        written_bytes = 0;
        written_bytes = sendto(socket, &pack, pack_len, MSG_CONFIRM, servaddr, servaddr_len);

    } while ( written_bytes != pack_len );

    // waiting for response to happen

    
    read_bytes = 0;
    // occhio al puntatore della size della socket
    read_bytes = recvfrom(socket, response, 1024, MSG_WAITALL, servaddr, &servaddr_len);

    printf("%s\n", response);
    if ( strncmp( response, EXISTS_ALREADY, read_bytes) == 0 )
    {
        return -1;
    } else if ( strncmp( response, CREATED_USER, read_bytes) == 0 ) {

        return 0;
    } else {
        printf("Questo branch non dovrebbe apparire, qualcosa di terribile è accaduto\n");
        return -10;
    }
        



}

int login_command( int socket, MyInfo * me, struct sockaddr * servaddr, socklen_t servaddr_len )
{

    printf("[2] %sLOGIN PROCEDURE%s\n", CYAN,END_COLOR);

    size_t pack_len = sizeof(Packet);
    

    Packet pack = { 0 };
    pack.type = LOGIN;
    int uname_len = get_user_input("USERNAME 16 CHAR: ", pack.sender, UNAME_MAX_LEN);
    int pwd_len = get_user_input("PASSWORD 16 CHAR: ", pack.data, PWD_MAX_LEN);

    // send request to the server

    int written_bytes;
    do {
        written_bytes = 0;
        written_bytes = sendto(socket, &pack, pack_len, MSG_CONFIRM, servaddr, servaddr_len);
    } while( written_bytes != pack_len );

    // wait for response

    int read_bytes = 0;
    char response[1024] = { 0 };

    // occhio al puntatore della size dellla socket
    read_bytes = recvfrom(socket, response, 1024, MSG_WAITALL, servaddr, &servaddr_len);

    printf("%s\n", response);

    if ( strncmp(response, NOT_EXISTS_USER, read_bytes) == 0) {
        return -1;
    } else if (strncmp(response, WRONG_PASSWORD, read_bytes) == 0) {
        return -2;
    } else if ( strncmp( response, LOGGED_USER, read_bytes) == 0 ) {
        // fill me with data
        strncpy(me -> username, pack.sender, uname_len);
        strncpy(me -> password, pack.data, pwd_len);
        me -> connected = TRUE;
        return 0;
        
    } else {
        printf("This branch should not be printed, in any case..\n");
        return -10;
    }
}












/**
 * --------------------  Threads functioanlity --------------------------------
 */

void interactivity( void * args)
{
    /**
     * Activity of interactive thread
     */
    thread_args_t * targs = ( thread_args_t *)args;

    int ret;
    char command[COMMAND_MAX_LEN] = { 0 };

    while ( targs -> me -> connected )
	{
		printf("%sType your choice then press Enter%s\n[1] PRIVATE MESSAGE (PM)\n[2] SHOW PENDING MESSAGES (SPM)\n[3] LOGOUT\n",GREEN, END_COLOR);

		ret = get_user_input("PM or SPM or LOGOUT\n>> ", command, COMMAND_MAX_LEN);
		
		if ( strncmp(command, "PM", ret) == 0) {
			// ret = newuser_command( sockfd, &servaddr, len);
            ret = pm_command( targs -> socket, targs -> me, (struct sockaddr*)targs -> servaddr, targs -> servaddr_len, &(targs -> semaphore) );
            printf("return code : %d\n", ret);
			// printf("Private Message\n");

		} else if ( strncmp( command, "SPM", ret) == 0 ) {
			
			// printf("%sLogin is under implementation phase%s\n", RED, END_COLOR);
			ret = spm_command( targs -> inbox);
			// printf("Show Pending Messages\n");
            printf("return code %d\n", ret);
		
		} else if ( strncmp( command, "LOGOUT", ret) == 0 ) {
			
			ret = logout_command( targs -> socket, targs -> me, (struct sockaddr*)targs -> servaddr, targs -> servaddr_len);
		
		} else {
			printf("Qui non dovresti starci\n");
		}

	}
}

int pm_command(int socket, MyInfo * me, struct sockaddr * servaddr, socklen_t servaddr_len, sem_t * semaphore) 
{

    Packet pack = { 0 };
    pack.type = MESSAGE;

    int sender_len = strlen(me -> username);
    strncpy( pack.sender, me -> username, sender_len);
    get_user_input("RECIPIENT MAX 16 CHAR: ", pack.recipient, UNAME_MAX_LEN);
    get_user_input("MESSAGE : ", pack.data, DATA_MAX_LEN);

    int pack_len = sizeof(Packet);

    // send request to the server
    

    int written_bytes;
    do {
        written_bytes = 0;
        written_bytes = sendto(socket, &pack, pack_len, MSG_CONFIRM, servaddr, servaddr_len);
    } while( written_bytes != pack_len );

    

   return 0;



}

int spm_command( MessageBox * inbox )
{
    int i;
    //int last_read = inbox -> last_read;
    int size = inbox -> size;
    Packet * pack;
    if ( size == 0)
    {
        printf("No messages to be printed\n");
    } else {
        for ( i = 0; i < size; i++ )
        {
            pack = &(inbox -> message_buffer[i]);
            printf("%s %s : %s %s\n",CYAN, pack -> sender, pack -> data, END_COLOR);
            //inbox -> last_read = size - 1;

        }
    }
    
    return size;
}

void receiving( void * args )
{

    // NO PRINT OR INPUT FROM COMMAND LINE INSIDE HERE
    
    
    thread_args_t * targs = ( thread_args_t *)args;

    //Packet * pack; 
    int pack_len = sizeof(Packet);
    int read_bytes = 0;
    int idx;
    
    // Receive message

    while ( targs -> me -> connected )
    {

        // sem_wait( &(targs -> semaphore));
       
            read_bytes = 0;
            idx = targs -> inbox -> size;
            // memset( pack, 0, pack_len);
            //pack = (Packet*) calloc (1, sizeof(Packet));

            read_bytes = recvfrom( targs -> socket, &(targs->inbox->message_buffer[idx]), pack_len, MSG_WAITALL, (struct sockaddr*) targs -> servaddr, &(targs -> servaddr_len) );

            if ( strncmp((const char*)&(targs->inbox->message_buffer[idx]), DISCONNECTED_USER, read_bytes) == 0) {
                break;
            }
        
       

        // printf("PACKET\n    %d\n    %s\n    %s\n    %s\n\n\n", pack -> type, pack -> sender, pack -> recipient, pack -> data);
        //sem_wait( &(targs -> semaphore));
        
        targs -> inbox -> size += 1;

        //sem_post( &( targs -> semaphore));

       
        
    }
}

int logout_command( int socket, MyInfo * me, struct sockaddr * servaddr, socklen_t servaddr_len)
{
    Packet pack = { 0 };
    pack.type = DISCONNECT;
    int written_bytes;
    int pack_len = sizeof(Packet);
    int uname_len = strlen(me -> username);
    strncpy( pack.sender, me -> username, uname_len);

    written_bytes = sendto(socket, &pack, pack_len, MSG_CONFIRM, servaddr, servaddr_len);

    me -> connected = FALSE;
    printf("%s DISCONNECTED%s\n", RED, END_COLOR);
    return 0;
}