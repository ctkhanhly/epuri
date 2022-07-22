#include <stdio.h>
#include <string.h>
#include <simplenet/net.h>
#include <simplenet/client.h>
#include <simplenet/server.h>
#include <stdlib.h>

void client_thread(char* host, char* port)
{
    simplenet_conn_t* conn = simplenet_open_client_socket(host, port);
    printf("Connected to: %s:%s\n", host, port);
    simplenet_send_message(conn, NULL, 0, CLOSE_MESSAGE);
    simplenet_close_client_socket(conn);
    printf("Done client thread\n");
}

void server_thread(char* host, char* port)
{
    printf("Listening on: %s\n", port);
    simplenet_open_server_socket(host, port);
}

int main(int argc, char* argv[]) {
    if(strcmp(argv[1], "server") == 0)
    {
        server_thread(argv[2], argv[3]);
    }
    else if(strcmp(argv[1], "client") == 0)
    {
        client_thread(argv[2], argv[3]);
    }  
    exit(EXIT_SUCCESS);
}