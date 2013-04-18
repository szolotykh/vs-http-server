#include "server.h"



int main(int argc, char *argv[]){
	// Check number of arguments
	if(argc<2){
		printf("Use: %s port\n", argv[0]);
		exit(0);
	}

	printf("===== Http Server =====\n");

	int serverSocket;
	
	char strBuffer[1024];
	int n;// number of send or received bytes	
	int msgLen;
	int processLen;
	FILE *fp; // Pointer on file
	int i; // For index
	char fileBuffer[MAX_FILE_SIZE]; // Buffer to read file

	serverSocket = startHTTPServer(atoi(argv[1]));

	int clientSock;
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	
	while(1){
     	clientSock = accept(serverSocket, (struct sockaddr *) &clientAddr, &clientLen);
     	if(clientSock < 0){
        	printf("ERROR on accept\n");
		continue;
	}
	printf("Client accepted\n");

	// Receive client request
	struct request* clientReq = receiveRequest(clientSock);

	printf("Method: %s\n", clientReq->method);
	printf("Object: %s\n", clientReq->object);
	printf("Protocol: %s\n", clientReq->protocol);
	printf("Headers:\n%s\n", clientReq->headers);

	struct response* sResp;

	char* UpgradeHeader = getRequestHeaderValue(clientReq, "Upgrade");
	if(UpgradeHeader == NULL){

	// Create object path
	char* objectPath = (char*)malloc(strlen("./wwwFiles/") + strlen(clientReq->object)+1);
	strcpy(objectPath, "./wwwFiles/");
	strcat(objectPath, clientReq->object);

	// Open requested object
	fp = fopen(objectPath,"r");
	if(fp != NULL){
		for(i = 0; i<MAX_FILE_SIZE; i++){
			int c = getc(fp);
			if(c == EOF){
				fileBuffer[i]='\0';
				break;
			}
			fileBuffer[i] = (char)c;
		}
		fclose(fp); // Close file

		sResp = createResponse("HTTP/1.1", "200", "OK");
		addHeaderResponse(sResp, "Content-Type: text/html; charset=UTF-8");

		strBuffer[0] = '\0';
		sprintf(strBuffer, "Content-Length: %d", (int)strlen(fileBuffer));
		addHeaderResponse(sResp, strBuffer);
		addHeaderResponse(sResp, "Connection: close");

		addBodyResponse(sResp, fileBuffer);
	}else{
		sResp = pageNotFoundResponse();
		addBodyResponse(sResp, "<html><h1>Page not found</h1></html>");
	}
	free(objectPath);
	}else{
		char* SecWebSocketKeyHeader = getRequestHeaderValue(
			clientReq, "Sec-WebSocket-Key");
		char* SecWebSocketProtocolHeader = getRequestHeaderValue(
			clientReq, "Sec-WebSocket-Protocol");
		sResp = webSocketResponse(SecWebSocketKeyHeader, SecWebSocketProtocolHeader);
	}


	if(sendResponse(clientSock, sResp) < 0){
		printf("ERROR writing to socket");
		exit(1);
	}
	freeResponse(sResp);


	close(clientSock); // Close client socketstruct request* receiveRequest(int sock);
	printf("Client disconnected\n");

	}
 	close(serverSocket);
	printf("Socket closed\n");

	return 0;
}

int startHTTPServer(int port){
	// Create socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
     	if (sock < 0){
        	printf("ERROR opening socket\n");
		exit(1);
	}
	printf("Socket created\n");

	struct sockaddr_in sAddr;
    	memset(&sAddr, '0', sizeof(sAddr));
    	sAddr.sin_family = AF_INET;
    	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    	sAddr.sin_port = htons(port);
	
	// Bind
    	if(bind(sock, (struct sockaddr*)&sAddr, sizeof(sAddr)) < 0){
		printf("ERROR to bind\n");
		exit( 1 );
	}
	printf("bind\n");
	// Listen
	listen(sock, 5);
	printf("listen\n");

	return sock;
}

int sendData(int sock, char* data, int dataLen){
	int n = 0, i;
	for(i = 0; i < dataLen; i+=n){
		n = write(sock,data+i, dataLen-i);
		if(n < 0)
			return n;
	}
	return i;
}

int sendResponse(int sock, struct response* resp){
	char *responseStr = responseToBuffer(resp);
	printf("%s\n", responseStr); // For debug
	int n = sendData(sock, responseStr, strlen(responseStr));
	free(responseStr);
	return n;
}

struct request* receiveRequest(int sock){
	char buffer[BUFFER_SIZE];
	int receivedLen = 0;
	int stopInd = 0;
	char cBuffer;
	int n;
	char* stopChars = "\r\n\r\n";
	
	while(1){
		if(receivedLen == BUFFER_SIZE){
			// Add error checking
			return NULL;
		}
		n = read(sock, &cBuffer, 1);
	     	if(n < 0){
			// Add error checking
			//printf("ERROR reading from socket");
			return NULL;
		}
		buffer[receivedLen] = cBuffer;
		receivedLen++;
		if(stopChars[stopInd] == cBuffer){
			stopInd++;
			if(stopInd == 4)
				break;
		}
		else{
			stopInd = 0;
		}
	}

	// Create request header object from string
	return requestFromBuffer(buffer, receivedLen);
}



