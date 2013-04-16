#include "server.h"
#include "HTTPServer.h"



int main(int argc, char *argv[]){
	// Check number of arguments
	if(argc<2){
		printf("Use: %s port\n", argv[0]);
		exit(0);
	}

	printf("===== Http Server =====\n");

	int serverSocket;
	
	char buffer[BUFFER_SIZE];
	char cBuffer;
	char strBuffer[1024];
	char* stopChars = "\r\n\r\n";
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
     	clientSock = accept(serverSocket, 
                 (struct sockaddr *) &clientAddr, &clientLen);
	printf("Client accepted\n");
     	if(clientSock < 0){
        	printf("ERROR on accept\n");
		exit(1);
	}

	bzero(buffer,BUFFER_SIZE);
	int receivedLen = 0;	
	int stopInd = 0;
	while(1){
		if(receivedLen == BUFFER_SIZE)
			break; // handle this
		n = read(clientSock, &cBuffer, 1);
     		if(n < 0){
			printf("ERROR reading from socket");
			exit( 1 );
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
	struct request* clientReq = requestFromBuffer(buffer, receivedLen);
	printf("Method: %s\n", clientReq->method);
	printf("Object: %s\n", clientReq->object);
	printf("Protocol: %s\n", clientReq->protocol);
	printf("Headers: %s\n", clientReq->headers);
	
	struct response* sResp;

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
	free(objectPath)
;
	char *respBuf = responseToBuffer(sResp);
	freeResponse(sResp);

	printf("%s\n", respBuf);
	msgLen = strlen(respBuf);
	n = 0;
	for(i = 0; i < msgLen; i+=n){
		n = write(clientSock,respBuf+i, msgLen-i);
		if(n < 0){
			printf("ERROR writing to socket");
			exit(1);
		}
	}
	free(respBuf);
	close(clientSock);
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
