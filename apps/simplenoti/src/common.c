#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "scm_functions.h"
#include "common.h"

simplelogger_t simplenoti_logger;
__attribute__((constructor))  static void simplenoti_init()
{
    srand(time(NULL));
}

void print_error(char* fmt)
{
    fprintf(stderr, fmt, strerror(errno));
}

int close_socket_pair(int sockPair[2])
{
    if (close(sockPair[0]) == -1)
    {
        print_error("closeSocketPair-close-0: %s\n");
        return -1;
    }
    if (close(sockPair[1]) == -1)
    {
        print_error("closeSocketPair-close-1: %s\n");
        return -1;
    }
    return 0;
}