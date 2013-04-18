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
#include "HTTPServer.h"

#define BUFFER_SIZE 2048
#define MAX_FILE_SIZE 4096


/* Start http server
* Discrioption:
*	Create server socket, bind and start listen
* Parameters:
*	port - Port which server will listen
* Return:
*	Server socket
*/
int startHTTPServer(int port);

/* Send data to client
* Parameters:
*	sock - Client socket
*	data - Pointer on data to send
*	dataLen - Length of data
* Return:
*	Number of send bytes or -1
*/
int sendData(int sock, char* data, int dataLen);

/* Send response to client
* Parameters:
*	sock - Client socket
*	resp - Response structure to send
* Return:
*	Number of send bytes or -1
*/
int sendResponse(int sock, struct response* resp);

struct request* receiveRequest(int sock);

#endif
