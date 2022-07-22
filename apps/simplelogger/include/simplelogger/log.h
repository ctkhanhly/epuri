#pragma once

#ifndef SIMPLELOGGER_H
#define SIMPLELOGGER_H

#include <stdio.h>

typedef struct simplelogger
{
    FILE* fp;
} simplelogger_t;

void simplelogger_init(simplelogger_t* logger, char* path);
void simplelogger_log (simplelogger_t* logger, const char * format, ...);
void simplelogger_log_error(simplelogger_t* logger, char* fmt);
void simplelogger_close(simplelogger_t* logger);

#endif