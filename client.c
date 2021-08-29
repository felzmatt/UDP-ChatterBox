#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"
#include "server.h"
#include "client.h"

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

    printf("[1] %sLOGIN PROCEDURE%s\n", CYAN,END_COLOR);
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
    read_bytes = recvfrom(socket, response, 1024, MSG_WAITALL, servaddr, servaddr_len);

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