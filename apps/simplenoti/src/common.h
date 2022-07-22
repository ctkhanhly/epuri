#pragma once

#ifndef SIMPLENOTI_COMMON_H
#define SIMPLENOTI_COMMON_H

#include <simplelogger/log.h>

#define SIMPLENOTI_MESSAGE_LEN 75

extern simplelogger_t simplenoti_logger;
void print_error(char* fmt);
int close_socket_pair(int sockPair[2]);

#endif