#include <stdio.h>       // perror, snprintf
#include <unistd.h>      // close, write
#include <string.h>      // strlen, bzero
#include <time.h>        // time, ctime
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <sys/types.h>
#include <netdb.h>     // getaddrinfo, NI_MAXHOST, NI_MAXSERV
#include <signal.h>
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/select.h>
#include <signal.h> /* sigaction, SIGINT, SIGQUIT */
#include <errno.h>
#include <simplenet/net.h>
#include <simplenet/client.h>

extern simplelogger_t simplenet_logger;

simplenet_conn_t* simplenet_open_client_socket(char* server_addr, char* port)
{
    
    setbuf(stdout, NULL); // Set buf to null so we don't need to flush
                        // Use C stream and system call simultaneously
    simplelogger_init(&simplenet_logger, getenv("SIMPLENET_LOGGER_PATH"));
    struct addrinfo hints;
    struct addrinfo * possible_servers;

    /* 
        Create a structure to get all 
        possible ip address and port for 
        that matches the given
        server address and port number
    */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;//check when this is valid
                                //IPv4 or IPv6
    hints.ai_flags = AI_NUMERICSERV;
                                //Wildcard IP address; service name is numeric

    if(getaddrinfo(server_addr, port, &hints, &possible_servers) != 0)
    {
        simplelogger_log_error(&simplenet_logger, "Simplenet: getaddrinfo %s");
        exit(EXIT_FAILURE);

    }

    return simplenet_connect_to_server(possible_servers);
}

/*
    This function loop through all the socket address structure
    that matches hints, a given server ip address and port number
    and uses the first one that works to connect to server
    and starts chatting with the server
*/

simplenet_conn_t* simplenet_connect_to_server(struct addrinfo * possible_servers){
    int serverfd;
    struct addrinfo *rp;
    char addr_str[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    socklen_t addrlen = sizeof(struct sockaddr_storage);
    /* Get one address that works and connect to server */

    for(rp = possible_servers; rp != NULL; rp = rp->ai_next){

        serverfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(serverfd == -1)
        {
            continue;
        }

        /* Get client's IP address and port number */
        if(getnameinfo((struct sockaddr *) rp->ai_addr, addrlen,
            host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
            snprintf(addr_str, ADDRSTRLEN, "(%s, %s)", host, service);
        }
        else{
            snprintf(addr_str, ADDRSTRLEN, "(?UNKNOWN?)");
        }
        simplelogger_log(&simplenet_logger, "Simplenet: Inspecting %s\n", addr_str);

        if(connect(serverfd, rp->ai_addr, rp->ai_addrlen) != -1){
            simplelogger_log(&simplenet_logger, "Simplenet: Connection to %s\n", addr_str);
            break;
        }

        if( close(serverfd) == -1){
            simplelogger_log_error(&simplenet_logger, "Simplenet: close serverfd %s");
            exit(EXIT_FAILURE);
        }

    }

    if(rp == NULL){
        simplelogger_log(&simplenet_logger, "Simplenet: Could not connect to any address\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(possible_servers); /* Free the addrinfo structure once we finish */

    simplenet_conn_t* conn = (simplenet_conn_t*)calloc(1, sizeof(simplenet_conn_t));
    simplenet_init_connection(conn, serverfd);
    return conn;
}

int simplenet_close_client_socket(simplenet_conn_t* conn)
{
    simplenet_close_connection(conn);
    simplelogger_close(&simplenet_logger);
    free(conn);
    return 0;
}