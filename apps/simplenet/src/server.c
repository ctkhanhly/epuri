#include <stdio.h>       // perror, snprintf
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netdb.h>     // getaddrinfo, NI_MAXHOST, NI_MAXSERV
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/select.h>
#include <signal.h> /* sigaction, SIGINT, SIGQUIT */
#include <errno.h>
#include <simplenet/net.h>
#include <simplenet/server.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>

static int simplenet_wait_for_clients(int listenfd);
static int simplenet_find_ip_addr_and_listen(char* host, char* port);
static int simplenet_init_server(simplenet_server_sock_t* server);
static int simplenet_add_client(simplenet_server_sock_t* server, int clientfd);
static int simplenet_process_client(simplenet_conn_t* client);

extern simplelogger_t simplenet_logger;

int simplenet_open_server_socket(char* host, char* port) {
    
    setbuf(stdout, NULL); // Set buf to null so we don't need to flush
                    // Use C stream and system call simultaneously
    /* Listen on one of the workable ip address found from result */
    simplelogger_init(&simplenet_logger, getenv("SIMPLENET_LOGGER_PATH"));
    if(port == NULL) port = DEFAULT_PORT;
    int listenfd = simplenet_find_ip_addr_and_listen(host, port);

    /* Process each client connected to the listening socket */
    return simplenet_wait_for_clients(listenfd);
}

/*
    This function establish connections with clients
    iteratively, one after another for any connected
    client and chat with each client
*/

static int simplenet_wait_for_clients(int listenfd){
    socklen_t addrlen;
    int clientfd;
    char addr_str[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    struct sockaddr_storage client_addr;
    simplenet_server_sock_t server;
    simplenet_init_server(&server);
    
    for(;;){

        addrlen = sizeof(struct sockaddr_storage);
        clientfd = accept(listenfd, (struct sockaddr*) &client_addr, &addrlen);

        if(clientfd == -1){
            simplelogger_log_error(&simplenet_logger, "Simplenet: accept %s");
            exit(EXIT_FAILURE);
        }

        /* Get client's IP address and port number */
        if(getnameinfo((struct sockaddr *) &client_addr, addrlen,
            host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
            snprintf(addr_str, ADDRSTRLEN, "(%s, %s)", host, service);
        }
        else{
            snprintf(addr_str, ADDRSTRLEN, "(?UNKNOWN?)");
        }

        simplelogger_log(&simplenet_logger, "Simplenet: Connection from %s\n", addr_str);

        // Process 1 client at a time for this app
        simplenet_add_client(&server, clientfd);
    }
    return -1;
}


/*
    This function loop through all the socket address structure
    that matches hints, any (wildcard) ip address  from client  
    and a given port number server is listening on
    and uses the first one that works to connect to server
    and starts chatting with the server
*/

static int simplenet_find_ip_addr_and_listen(char* host, char* port)
{
    struct addrinfo hints;
    struct addrinfo * result;
    int listenfd;
    int optVal = 1;
    struct addrinfo * rp;

    /* 
    Create a structure to get all 
    possible ip address and port for 
    to use as server address and store the list
    of possible addresses in result
    */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;//check when this is valid
                                //IPv4 or IPv6
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
                                //Wildcard IP address; service name is numeric

    if(getaddrinfo(host, port, &hints, &result) != 0)
    {
        simplelogger_log_error(&simplenet_logger, "Simplenet: getaddrinfo %s");
        exit(EXIT_FAILURE);
    }
    
    for(rp = result; rp != NULL; rp = rp->ai_next){
        listenfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(listenfd == -1){
            continue;
        }

        /* This option is suggested by the book for TCP connection */
        if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) == -1){
            simplelogger_log_error(&simplenet_logger, "Simplenet: setsockopt %s");
            exit(EXIT_FAILURE);
        }

        if(bind(listenfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; //SUCCESS

        close(listenfd);

    }


    if(rp == NULL){
        simplelogger_log(&simplenet_logger, "Simplenet: Could not bind any address\n");
        exit(EXIT_FAILURE);
    }

    /* listen on the established socket to any connecting client */
    if(listen(listenfd, BACKLOG) == -1 ){
        simplelogger_log_error(&simplenet_logger, "Simplenet: listen %s");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result); /* Free the addrinfo structure once we finish */
    return listenfd;
    
}

static int simplenet_init_server(simplenet_server_sock_t* server)
{
    server->connection = NULL;
    return 0;
}

static int simplenet_add_client(simplenet_server_sock_t* server, int clientfd)
{
    server->connection = (simplenet_conn_t*) calloc(1, sizeof(simplenet_conn_t));
    simplenet_init_connection(server->connection, clientfd);
    int result = simplenet_process_client(server->connection);
    free(server->connection);
    server->connection = NULL;
    return result;
}

static int simplenet_process_client(simplenet_conn_t* client)
{
    int num_read, num_bytes;
    unsigned char buf[SIMPLENET_MAX_PACKET_SIZE];
    SIMPLENET_MESSAGE_TYPE message_type;
    int fd;
    unsigned message_len, id;
    for(;;)
    {
        num_read = read(client->sockfd, buf, SIMPLENET_MESSAGE_HEADER_LEN);
        if(num_read <= 1) goto out;
        message_type = (SIMPLENET_MESSAGE_TYPE)buf[0];
        id = simplenet_unpack_int(buf+1, 4);
        message_len = simplenet_unpack_int(buf+5, 4);
        num_bytes = 0;
        
        switch(message_type)
        {
        case PRIVATE_KEY_MESSAGE:
            if ((fd = open(SIMPLENET_PRIVATE_KEY_OUT, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
                simplelogger_log(&simplenet_logger, "Simplenet: Error opening file %s\n", SIMPLENET_PRIVATE_KEY_OUT);
                goto out;
            }
            break;
        case CERT_MESSAGE:
            if ((fd = open(SIMPLENET_CERT_OUT, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
                simplelogger_log(&simplenet_logger, "Simplenet: Error opening file %s\n", SIMPLENET_CERT_OUT);
                goto out;
            }
            break;
        case CACERT_MESSAGE:
            if ((fd = open(SIMPLENET_CACERT_OUT, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
                simplelogger_log(&simplenet_logger, "Simplenet: Error opening file %s\n", SIMPLENET_CERT_OUT);
                goto out;
            }
            break;
        case CLOSE_MESSAGE:
            sleep(1);
            return simplenet_close_connection(client);
        default:
            simplelogger_log(&simplenet_logger, "Simplenet: Unexpected message type: %d\n", message_type);
        }
        
        while(num_bytes < message_len)
        {
            num_read = read(client->sockfd, buf, message_len-num_bytes);
            if(num_read <= 0)
            {
                simplelogger_log(&simplenet_logger, "Simplenet: Error reading from remote %d\n", num_read);
                goto out;
            }
            num_bytes += num_read;
            write(fd, buf, num_read);
        }
        close(fd);
        
        if(num_bytes != message_len)
        {
            simplelogger_log(&simplenet_logger, "Simplenet: Messages length don't match (expected: %d, got: %d)\n", message_len, num_read);
        }
        simplelogger_log(&simplenet_logger, "Simplenet: Received message type: %d, id: %u, written: %d, message_len: %d\n", message_type, id, num_bytes, message_len);
    }
out:
    simplenet_close_connection(client);
    simplelogger_close(&simplenet_logger);
    return -1;
}
