#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <simplelogger/log.h>

void simplelogger_init(simplelogger_t* logger, char* path)
{
    if(logger == NULL) return;
    if(path == NULL)
        logger->fp = NULL;
    else logger->fp = fopen(path, "w+");
}

void simplelogger_log (simplelogger_t* logger, const char * format, ...)
{
    if(logger == NULL || logger->fp == NULL) return;
    va_list args;
    va_start (args, format);
    vfprintf (logger->fp, format, args);
    va_end (args);
    fflush(logger->fp);
}

void simplelogger_log_error(simplelogger_t* logger, char* format)
{
    if(logger == NULL || logger->fp == NULL) return;
    simplelogger_log(logger, format, strerror(errno));
}

void simplelogger_close(simplelogger_t* logger)
{
    if(logger == NULL) return;
    fclose(logger->fp);
    logger->fp = NULL;
}