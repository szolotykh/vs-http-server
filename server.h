#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>

#define BUFFER_SIZE 2048
#define MAX_FILE_SIZE 4096

int startHTTPServer(int port);

#endif
