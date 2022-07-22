#include <stdio.h>       // perror, snprintf
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE, getenv */
#include <signal.h>
#include <errno.h>
#include <simplenet/net.h>
#include <time.h>
#include <assert.h>

static unsigned char buf[SIMPLENET_MAX_PACKET_SIZE+11];
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

simplelogger_t simplenet_logger;
__attribute__((constructor))  static void simplenet_init()
{
    srand(time(NULL));
    simplelogger_init(&simplenet_logger, getenv("SIMPLENET_LOGGER_PATH"));
}

int simplenet_pack_int(unsigned num, unsigned char* buf, int buf_len)
{
    assert(buf_len == 4);
    const static unsigned byte = 0xffff;
    buf[0] = num & byte;
    buf[1] = (num >> 8) & byte;
    buf[2] = (num >> 16) & byte;
    buf[3] = (num >> 24) & byte;
    return 0;
}

unsigned simplenet_unpack_int(unsigned char* buf, int buf_len)
{
    assert(buf_len == 4);
    unsigned num = 0;
    num |= buf[3];
    num <<= 8;
    num |= buf[2];
    num <<= 8;
    num |= buf[1];
    num <<= 8;
    num |= buf[0];
    return num;
}

static void simplenet_construct_packet(SIMPLENET_MESSAGE_TYPE type, int id, unsigned char* message, unsigned message_len, unsigned num_write)
{
    unsigned buf_len = 0;
    buf[0] = type;
    buf_len += 1;
    simplenet_pack_int(id, buf+buf_len, 4);
    buf_len += 4;
    simplenet_pack_int(message_len, buf+buf_len, 4);
    buf_len += 4;
    memcpy(buf+buf_len, message, num_write);
}

int simplenet_init_connection(simplenet_conn_t* conn, int sockfd)
{
    conn->sockfd = sockfd;
    return 0;
}

int simplenet_send_message(simplenet_conn_t* conn, unsigned char* message, unsigned message_len, SIMPLENET_MESSAGE_TYPE type)
{
    unsigned id = rand();
    int total_written = 0, written_bytes, num_write;
    simplenet_construct_packet(type, id, message, message_len, 0);
    written_bytes = write(conn->sockfd, buf, SIMPLENET_MESSAGE_HEADER_LEN);
    while(total_written < message_len)
    {
        num_write = MIN(message_len-total_written, SIMPLENET_MAX_PACKET_SIZE);
        written_bytes = write(conn->sockfd, message+total_written, num_write);
        
        if(written_bytes == -1) {
            simplelogger_log_error(&simplenet_logger, "Simplenet: simplenet_send_message write %s");
            return -1;
        }

        total_written += written_bytes;
    }
    return total_written;
}

int simplenet_read_message(simplenet_conn_t* conn, unsigned char* buf, unsigned buf_len)
{
    return read(conn->sockfd, buf, buf_len);
}

int simplenet_close_connection(simplenet_conn_t* conn)
{
    return close(conn->sockfd);
}


