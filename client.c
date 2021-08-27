#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"
#include "server.h"
#include "client.h"

void newuser_command()
{
    /**
     * This function is invoked when a not logged user prompt 1 in the terminal
     * When the user is logged this function is no longer available
     * This is the only function that can set CONNECTED = TRUE
     * 
     * This function MUST return an integer indicating the result of requested operation
     */

    // ask user for his username and his password
    char username[UNAME_MAX_LEN] = {0};
    char password[PWD_MAX_LEN] = {0};

    printf("[1] %sLOGIN PROCEDURE%s\n", CYAN,END_COLOR);
    
}