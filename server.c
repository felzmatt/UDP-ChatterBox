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



#include "mylist.h"
#include "server.h"

User * find_user_by_username( list_t * users, const char * username )
{
    // scan all the list in search of user with username == username if it exist
    int size = users -> items;
    int uname_len = strlen(username);
    list_node_t * node = users -> head;
    User * user = (User*) node;
    int i;
    for ( i = 1; i <= size; i++)
    {
        if ( strncmp(username, user -> username, uname_len) == 0 ){
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

void handle_new_user(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t  sock_len, list_node_ops * ops)
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
    printf("Handling NEWUSER\n");
    char username[UNAME_MAX_LEN] = {0};
    char password[PWD_MAX_LEN] = {0};
    
    int uname_len = strlen( packet -> sender);
    int pwd_len = strlen( packet -> data );
    
    strncpy( username, packet -> sender, uname_len );
    strncpy( password, packet -> data, pwd_len );
    
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
        strncpy( added_user -> username, username, uname_len);
        strncpy( added_user -> password, password, pwd_len);
        added_user -> online = FALSE;
        init_list_node( &(added_user -> node), ops);
        append_list_node(users, &(added_user -> node) );

        
        do {
            written_bytes = 0;
            written_bytes = sendto(socket, CREATED_USER, confirm_len, MSG_CONFIRM, client_addr, sock_len);
        } while ( written_bytes != confirm_len);
    }
    

}

void handle_login(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t sock_len, list_node_ops * ops)
{
    /**
     * Handles requests of LOGIN
     * REMEMBER THAT CLIENT IS BLOCKED ON THIS REQ UNTIL A RESPONSE IS RECEIVED
     * The packet is composed as follow
     * type = LOGIN
     * sender = <username>
     * recipient = <all - zeros>
     * data = <password>
     * The parsing of message is same of others ( I should have write a function !)
     */
    printf("Handling LOGIN\n");
    char username[UNAME_MAX_LEN] = {0};
    char password[PWD_MAX_LEN] = {0};
    
    int uname_len = strlen( packet -> sender);
    int pwd_len = strlen( packet -> data );
    
    strncpy( username, packet -> sender, uname_len );
    strncpy( password, packet -> data, pwd_len );
    // int username_len = strlen(username);
    int password_len = strlen(password);
    
    User * ret = find_user_by_username( users, username);
    
    int written_bytes = 0;
    
    int error_len = strlen(NOT_EXISTS_USER);
    int wrongpwd_len = strlen(WRONG_PASSWORD);
    int logged_len = strlen(LOGGED_USER);

    if (  ret == NULL ) 
    {
        // users is not registered
        do {
            written_bytes = 0;
            written_bytes = sendto(socket, NOT_EXISTS_USER, error_len, MSG_CONFIRM, client_addr, sock_len);
        } while ( written_bytes != error_len);

    } else {
        // user has been found in list parsed from db
        // now it is mandatory to check if password is correct
        if ( strncmp( ret -> password, password, password_len) == 0 )
        {
            // user has demonstrated to be who claims to be, put him/she online and change flag in list
            do {
                written_bytes = 0;
                written_bytes = sendto(socket, LOGGED_USER, logged_len, MSG_CONFIRM, client_addr, sock_len);
            } while ( written_bytes != logged_len);
            // NB: we are also copying information about client connection
            memcpy( &(ret -> client_address), client_addr, sock_len);
            // set flag online
            ret -> online = TRUE;

        } else {

            // password is wrong, sorry
            do {
                written_bytes = 0;
                written_bytes = sendto(socket, WRONG_PASSWORD, wrongpwd_len, MSG_CONFIRM, client_addr, sock_len);
            } while ( written_bytes != wrongpwd_len);

        }

    }
}

void handle_message(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t sock_len, list_node_ops * ops)
{
    /**
     * Handle the MESSAGE request
     * also here the client interactivity is blocked untile he receives one among the possible response to this message
     * the  packet is composed as follow
     * type = MESSAGE
     * sender = <username of sender>
     * recipient = <username of recipient>
     * data = <body of message>
     */ 
    printf("Handling MESSAGE\n");
    // initialize variables
    char sender[UNAME_MAX_LEN] = {0};
    char recipient[UNAME_MAX_LEN] = {0};
    char data[DATA_MAX_LEN] = {0};
    // copy data from packet
    // IN CASE OF ERROR COME HERE AND SEE THE strncpy
    int sender_len = strlen(packet ->sender);
    int recipient_len = strlen(packet -> recipient);
    strncpy(recipient, packet -> recipient, recipient_len);
    strncpy(sender, packet -> sender, sender_len);
    strncpy(data, packet -> data, DATA_MAX_LEN);

    // int sender_len = strlen(sender);
    // int recipient_len = strlen(recipient);
    int data_len = strlen(data);
    
    // looking for recipient
    User * ret = find_user_by_username( users, recipient );

    printf("Ho trovato il recipient %s\n", recipient);
    printUser((list_node_t*)ret);
    
    int written_bytes = 0;
    
    int not_found_user_len = strlen( USER_NOT_FOUND );
    int not_online_user_len = strlen( USER_NOT_ONLINE );
    int message_sent_len = strlen( SENT_MESSAGE );

    Packet pack = { 0 };
    pack.type = MESSAGE;
    strncpy( pack.sender, sender, sender_len);
    strncpy( pack.recipient, recipient, recipient_len);
    strncpy( pack.data, data, data_len);

    int pack_len = sizeof(Packet);

    // behaviour depends on existence and connection of the indended recipient
    if ( ret == NULL )
    {
        // user is not registered

        do {
            written_bytes = 0;
            written_bytes = sendto(socket, USER_NOT_FOUND, not_found_user_len, MSG_CONFIRM, client_addr, sock_len);
        } while ( written_bytes != not_found_user_len);
    } else {
        
        if ( ret -> online == FALSE )
        {
            // user not connected
            do {
                written_bytes = 0;
                written_bytes = sendto(socket, USER_NOT_ONLINE, not_online_user_len, MSG_CONFIRM, client_addr, sock_len);
            } while ( written_bytes != not_online_user_len);
        
        } else {
            // user is connected

            printf("try to send message\n");

            // try to send message to recipient
            // note that we are sending to the address of ret
            /*
            do {
                written_bytes = 0;
                written_bytes = sendto( socket, &pack, pack_len, MSG_CONFIRM, &(ret -> client_address), sock_len );
            } while ( written_bytes != pack_len);
            */
            // at this point we have to unlock the waiting sender

            do {
                written_bytes = 0;
                written_bytes = sendto( socket, &pack, pack_len, MSG_CONFIRM, &(ret -> client_address), sock_len );
            } while ( written_bytes != pack_len);



        }
    } 
    printf("sent message\n");
}

void handle_disconnect(int socket, list_t * users, Packet * packet, struct sockaddr * client_addr, socklen_t sock_len, list_node_ops * ops)
{
    char username[UNAME_MAX_LEN] = { 0 };
    int uname_len = strlen(packet -> sender);
    int written_bytes = 0;
    int disconn_len = strlen(DISCONNECTED_USER);
    // printf("DEBUG %s %lu\n", packet -> sender, uname_len);
    strncpy(username, packet -> sender, UNAME_MAX_LEN);
    User * user = find_user_by_username( users, packet -> sender);
    // printUser(user);
    if ( user == NULL ) {
        printf("This string should never be printed.. error in finding user\n");
        return;
    }
    user -> online = FALSE;
    written_bytes = sendto(socket, DISCONNECTED_USER, disconn_len, MSG_CONFIRM, client_addr, sock_len);
    printf("Successfully disconnected user\n");
    return;

}