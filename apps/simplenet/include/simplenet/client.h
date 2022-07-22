#pragma once

#ifndef SIMPLENET_CLIENT_H
#define SIMPLENET_CLIENT_H

#include <simplenet/net.h>
#include <sys/types.h>

simplenet_conn_t* simplenet_open_client_socket(char* server_addr, char* port);
int simplenet_close_client_socket(simplenet_conn_t* conn);
static simplenet_conn_t* simplenet_connect_to_server(struct addrinfo * possible_servers);

#endif