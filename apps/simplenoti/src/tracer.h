#pragma once

#ifndef SIMPLENOTI_SANDBOX_H
#define SIMPLENOTI_SANDBOX_H

#include <seccomp.h>

#define BUF_SIZE 4096

void tracer_run(int sock_pair[2], char* expected_host, char* expected_port);

#endif