/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

/* scm_functions.h
   Functions to exchange ancillary data over a UNIX domain socket.
*/
#pragma once        /* Prevent accidental double inclusion */

#ifndef SCM_FUNCTIONS_H
#define SCM_FUNCTIONS_H

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <errno.h>

int sendfd(int sockfd, int fd);
int recvfd(int sockfd);

#endif