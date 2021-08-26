#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>



#include "list.h"
#include "server.h"

User * find_user_by_username( list_t * users, const char * username )
{
    // scan all the list in search of user with username == username if it exist
    int size = users -> items;
    list_node_t * node = users -> head;
    User * user = (User*) node;
    int i;
    for ( i = 1; i <= size; i++)
    {
        if ( strcmp(username, user -> username) == 0 ){
            return user;
        }
        node = node -> next;
        user = (User*)node;
    }
    return NULL;


}

void printUser ( list_node_t * node )
{
    User* user = (User*)node;
    printf("----user-----\n username %s\n password %s\n online %d\n-----------\n", user->username, user->password, user->online);
}

int loadUsers( list_t * users , const char * filepath , list_node_ops * ops)
{
    int ret;
    int i = 0;
    int fd = open( filepath, O_CREAT, 0660);
    int read_bytes = 0;
    User * user;
    char username[UNAME_MAX_LEN] = {0};
    char password[PWD_MAX_LEN] = {0};
    int stop = 0;
    while(!stop)
    {   
        read_bytes = 0;
        user = (User*) calloc(1,sizeof(User));
        memset(username, 0, UNAME_MAX_LEN);
        memset(password, 0, PWD_MAX_LEN);

        // parse the username and the password 1 byte
        do {
            ret = read(fd, username + read_bytes, 1);
            if ( ret == -1 && errno == EINTR) continue;
            if ( ret == -1 ) handle_error("Error reading from file");
            if (ret == 0) 
            return i;
        } while( username[read_bytes++] != ' ');

        strncpy(user -> username, username, read_bytes - 1);
        read_bytes = 0;

        do {
            ret = read(fd, password + read_bytes, 1);
            if ( ret == -1 && errno == EINTR) continue;
            if ( ret == -1 ) handle_error("Error reading from file");
            if (ret == 0) 
            return i;
        } while( password[read_bytes++] != '\n');

        strncpy(user -> password, password, read_bytes - 1);
        
        user -> online = FALSE;

        init_list_node( &(user -> node), ops );
        append_list_node(users, &(user -> node) );

        i++;
    }

    return i;
}


void print_packet( Packet * pack)
{
    printf("PACKET\n    %d\n    %s\n    %s\n    %s\n\n\n", pack -> type, pack -> sender, pack -> recipient, pack -> data);
}

int handle_new_user(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t  sock_len, list_node_ops * ops)
{  
    /**
     * Handles the request of type newuser
     * REMEMBER THAT CLIENT IS BLOCKED UNTIL RESPONSE IS RECEIVED
     * The packet is composed as follow
     * type = NEWUSER
     * sender = <username>
     * recipient = <all-zero>
     * data = <password>
     * 
     * 1) grab the <username> and check that no other users with same exists in the list
     *    to accomplish this we need a function that search in the list by username and return NULL if not find
     * 2)
     *      a) users not exist already:
     *          create a User object with <username> , <password> and online = FALSE
     *          send CREATED_USER to client 
     *      b) users exists already:
     *          send ALREADY_EXIST to client 
     * FINISH, we need pointer to list of users, pointer to packet received, and client address to send result
     * 
     */
    char username[UNAME_MAX_LEN] = {0};
    char password[PWD_MAX_LEN] = {0};
    sprintf(username, "%s", packet -> sender);
    sprintf(password, "%s", packet -> data);
    int username_len = strlen(username);
    int password_len = strlen(password);
    
    User * ret = find_user_by_username( users, username);
    
    int written_bytes = 0;
    
    int error_len = strlen(EXISTS_ALREADY);
    int confirm_len = strlen(CREATED_USER);
    
    
    if ( ret != NULL )
    {
        do {
            written_bytes = 0;
            written_bytes = sendto(socket, EXISTS_ALREADY, error_len, MSG_CONFIRM, client_addr, sock_len);
        } while ( written_bytes != error_len);
    } else {
        // crea utente e rispondi con ACK
        User * added_user = ( User* ) calloc (1, sizeof(User));
        strncpy( added_user -> username, username, username_len);
        strncpy( added_user -> password, password, password_len);
        added_user -> online = FALSE;
        init_list_node( &(added_user -> node), ops);
        append_list_node(users, &(added_user -> node) );

        
        do {
            written_bytes = 0;
            written_bytes = sendto(socket, CREATED_USER, confirm_len, MSG_CONFIRM, client_addr, sock_len);
        } while ( written_bytes != confirm_len);
    }
    return 0;

}