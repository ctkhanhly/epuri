#pragma once

#ifndef SIMPLENET_NET_H
#define SIMPLENET_NET_H

#include <simplelogger/log.h>

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif
#ifndef NI_MAXSERV
#define NI_MAXSERV 32
#endif

#define LOCAL_ADDR "127.0.0.1"
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
#define DEFAULT_PORT "5000"
#define BACKLOG 50

#define SIMPLENET_MAX_PACKET_SIZE 1024
#define SIMPLENET_MAX_MESSAGE_LEN 4096
#define SIMPLENET_MESSAGE_HEADER_LEN 9
#define SIMPLENET_LOGGER_MESSAGE_LEN 75


typedef enum {
    PRIVATE_KEY_MESSAGE = 1,
    CERT_MESSAGE = 2,
    CACERT_MESSAGE = 3,
    CLOSE_MESSAGE = 4,
} SIMPLENET_MESSAGE_TYPE;

extern simplelogger_t simplenet_logger;

typedef struct connection
{
    int sockfd;
} simplenet_conn_t;

int simplenet_init_connection(simplenet_conn_t* conn, int sockfd);
int simplenet_send_message(simplenet_conn_t* conn, unsigned char* message, unsigned message_len, SIMPLENET_MESSAGE_TYPE type);
int simplenet_read_message(simplenet_conn_t* conn, unsigned char* buf, unsigned buf_len);
int simplenet_close_connection(simplenet_conn_t* conn);
unsigned simplenet_unpack_int(unsigned char* buf, int buf_len);
int simplenet_pack_int(unsigned num, unsigned char* buf, int buf_len);

#endif