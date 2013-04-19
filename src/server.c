#include "server.h"



int main(int argc, char *argv[]){
	// Check number of arguments
	if(argc<2){
		printf("Use: %s port\n", argv[0]);
		exit(0);
	}

	printf("===== Http Server =====\n");

	// Start HTTP Server
	int serverSocket = startHTTPServer(atoi(argv[1]));
	if(serverSocket < 0){
		exit(0);
	}

	
	while(1){
		unsigned char usingProtocol = HTTP;

		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
	     	int clientSock = accept(
			serverSocket, (struct sockaddr *) &clientAddr, &clientLen);
	     	if(clientSock < 0){
			printf("ERROR on msgLenaccept\n");
			continue;
		}
		printf("Client accepted\n");

		// Receive client request
		struct request* clientReq = receiveRequest(clientSock);
		// Print client request
		printf("Method: %s\n", clientReq->method);
		printf("Object: %s\n", clientReq->object);
		printf("Protocol: %s\n", clientReq->protocol);
		printf("Headers:\n%s\n", clientReq->headers);

		struct response* sResp;

		char* UpgradeHeader = getRequestHeaderValue(clientReq, "Upgrade");
		if(UpgradeHeader == NULL){

		// Create object path
		char* objectPath = strmerge("./wwwFiles/", clientReq->object);
		// Open requested object file
		char *fileBuffer = fileToString(objectPath);
		// Free object path
		free(objectPath);

		// Check if the file
		if(fileBuffer != NULL){

			sResp = createResponse("HTTP/1.1", "200", "OK");
			addHeaderResponse(sResp, "Content-Type: text/html; charset=UTF-8");

			char* strLength = intToStr((int)strlen(fileBuffer));
			addHeaderResponsePair(sResp, "Content-Length", strLength);
			addHeaderResponse(sResp, "Connection: close");

			addBodyResponse(sResp, fileBuffer);

			free(fileBuffer);
			free(strLength);
		}else{
			sResp = pageNotFoundResponse();
			addBodyResponse(sResp, "<html><h1>Page not found</h1></html>");
		}
	
		}else{
			char* SecWebSocketKeyHeader = getRequestHeaderValue(
				clientReq, "Sec-WebSocket-Key");
			char* SecWebSocketProtocolHeader = getRequestHeaderValue(
				clientReq, "Sec-WebSocket-Protocol");
			sResp = webSocketResponse(
				SecWebSocketKeyHeader,
				SecWebSocketProtocolHeader);

			usingProtocol = WEB_SOCKET;
		}


		if(sendResponse(clientSock, sResp) < 0){
			printf("ERROR writing to socket");
			exit(1);
		}
		freeResponse(sResp);

		if(usingProtocol == WEB_SOCKET){
			// Start WebSocket communication
			printf("Start WebSocket communication\n");
			struct wsFrame* wsMsg = receiveWSMessage(clientSock);
			printf("Length: %d\n", wsMsg->len);
			if(wsMsg->mask == 1)
				printf("Masking-key: %s\n", wsMsg->maskingKey);
			if(wsMsg->len > 0)
				printf("Data: %s\n", wsMsg->data);
			free(wsMsg);
		}

		close(clientSock); // Close client socket
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
		return sock;
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
		return -1;
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
	char *responseStr = responseToString(resp);
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

char* fileToString(char* path){
	FILE *fp = fopen(path,"r"); // Open file
	if(fp == NULL)
		return NULL;
	// Obtain file size
	fseek(fp, 0L, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	// Allocate memory
	char *str = (char*)malloc(fileSize+1);
	
	//Reading file
	int i;
	for(i = 0; i<fileSize; i++){
		str[i] = (char)getc(fp);
	}
	str[fileSize]='\0';
	fclose(fp); // Close file
	return str;
}
struct wsFrame* receiveWSMessage(int sock){
	struct wsFrame* msg = (struct wsFrame*)malloc(sizeof(struct wsFrame));
	int n, i;
	unsigned char rByte;

	// Read first byte
	n = read(sock, &rByte, 1);
	if(n < 0)
		return NULL;
	msg->fin = rByte>>7;
	unsigned char rsv = rByte & 0x70;
	if(rsv != 0)
		return NULL;
	msg->opcode = rByte&0x0F;

	// Read second byte
	n = read(sock, &rByte, 1);
	if(n < 0)
		return NULL;

	msg->mask = rByte & 0x80;
	msg->mask = msg->mask >> 7;

	rByte = rByte & 0x7F;

	printf("rByte: %d\n", rByte);
	if(rByte < 126){
		msg->len = (int)rByte;
	}else{
		int lenSize;
		if(rByte == 126)
			lenSize = 2;
		else
			lenSize = 4;

		char *d = (char*)&msg->len;
		for(i = 0; i < lenSize; i+=n){
			n = read(sock, d+i, lenSize-i);
			if(n < 0)
				return NULL;
		}
		msg->len = ntohl(msg->len);
		if(rByte == 126)
			msg->len = msg->len>>16;
	}
	if(msg->mask == 1){
		msg->maskingKey = (char*)malloc(5);
		for(i = 0; i < 4; i+=n){
			n = read(sock, msg->maskingKey+i, 4-i);
			if(n < 0)
				return NULL;
		}
		msg->maskingKey[4] = '\0';
	}
	if(msg->len > 0){
		msg->data = (char*)malloc(msg->len);
		for(i = 0; i < msg->len; i+=n){
			n = read(sock, msg->data+i, msg->len-i);
			if(n < 0)
				return NULL;
		}
		msg->data[msg->len] = '\0';
		if(msg->mask == 1){
			for(i = 0; i < msg->len; i++){
				msg->data[i] = msg->data[i] ^ msg->maskingKey[i%4];
			}
		}
	}
	return msg;
}







