#pragma once

#ifndef SIMPLENET_SERVER_H
#define SIMPLENET_SERVER_H

#include <simplenet/net.h>

#define SIMPLENET_PRIVATE_KEY_OUT "client.key"
#define SIMPLENET_CERT_OUT "client.crt"
#define SIMPLENET_CACERT_OUT "ca.crt"

typedef struct server_sock
{
    simplenet_conn_t* connection;
} simplenet_server_sock_t;

int simplenet_open_server_socket(char* host, char* port);

#endif