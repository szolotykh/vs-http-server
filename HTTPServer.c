#include "HTTPServer.h"

// Request functions

struct request* requestFromBuffer(char* buf, int len){
	struct request* req = (struct request*)malloc(sizeof(struct request));
	char* ch_end;
	char* ch_start = buf;
	int length;

	// Method
	
	ch_end = (char*)memchr(ch_start,' ',len);
	length = ch_end - ch_start;
	req->method = (char*)malloc(length+1);
	memcpy(req->method, buf, length);
	req->method[length] = '\0';
	ch_start = ch_end+2;

	// Object
	ch_end = (char*)memchr(ch_start,' ',len-(ch_start-buf));
	length = ch_end - ch_start;
	req->object = (char*)malloc(length+1);
	memcpy(req->object, ch_start, length);
	req->object[length] = '\0';
	ch_start = ch_end+1;

	// Protocol
	ch_end = (char*)memchr(ch_start,'\r',len-(ch_start-buf));
	length = ch_end - ch_start;
	req->protocol = (char*)malloc(length+1);
	memcpy(req->protocol, ch_start, length);
	req->protocol[length] = '\0';
	ch_start = ch_end+2;

	//Headers
	length = len-(ch_start-buf)-2;
	req->headers = (char*)malloc(length+1);
	memcpy(req->headers, ch_start, length);
	req->headers[length] = '\0';

	return req;
}

char* getRequestHeaderValue(struct request* req, char* name){
	char* searchName = (char*)malloc(strlen(name)+3);
	strcpy(searchName, name);
   	strcat(searchName, ": ");

	char* start = strstr(req->headers, searchName);
	if(start == NULL){
		free(searchName);
		return NULL;
	}
	char* end = strstr(start, "\r\n");
	start += strlen(searchName);
	

	int valueLen = end-start;
	char* valueStr = (char*)malloc(valueLen + 1);
	memcpy(valueStr, start,valueLen);
	valueStr[valueLen] = '\0';
	free(searchName);
	return valueStr;
}

void freeRequest(struct request* req){
	free(req->method);
	free(req->object);
	free(req->protocol);
	free(req->headers);
	free(req);
}

// Response functions

struct response* createResponse(char* protocol, char *code, char *msg){
	struct response* resp = (struct response*)malloc(sizeof(struct response));
	// Protocol
	resp->protocol = (char*)malloc(strlen(protocol)+1);
	strcpy(resp->protocol, protocol);
	// Code
	resp->code = (char*)malloc(strlen(code)+1);
	strcpy(resp->code, code);
	// Msg
	resp->msg = (char*)malloc(strlen(msg)+1);
	strcpy(resp->msg, msg);
	// Headers
	resp->headers = (char*)malloc(1);
	strcpy(resp->headers, "");
	// Body
	resp->body = (char*)malloc(1);
	strcpy(resp->body, "");
	return resp;	
}

void addHeaderResponse(struct response* resp, char* header){
	char* str = (char*)malloc(strlen(resp->headers) + strlen(header)+1+2);
	strcpy(str, resp->headers);
	strcat(str, header);
	strcat(str, "\r\n");
	free( resp->headers );
	resp->headers = str;
}
void addHeaderResponsePair(struct response* resp, char* name, char* value){
	char* str = (char*)malloc(strlen(resp->headers) + strlen(name)+2+strlen(value)+1+2);
	strcpy(str, resp->headers);
	strcat(str, name);
	strcat(str, ": ");
	strcat(str, value);
	strcat(str, "\r\n");
	free( resp->headers );
	resp->headers = str;
}


void addBodyResponse(struct response* resp, char* body){
	char* str = (char*)malloc(strlen(resp->body) + strlen(body)+1);
	strcpy(str, resp->body);
	strcat(str, body);
	free( resp->body );
	resp->body = str;
}

char* responseToBuffer(struct response* resp){
	int size = strlen(resp->protocol)+1+strlen(resp->code)+1+strlen(resp->msg)+2+
		strlen(resp->headers)+2+strlen(resp->body);
	char* buf = (char*)malloc( size );
	strcpy(buf, resp->protocol);
	strcat(buf, " ");
	strcat(buf, resp->code);
	strcat(buf, " ");
	strcat(buf, resp->msg);
	strcat(buf, "\r\n");

	if( strlen(resp->headers) != 0 )
		strcat(buf, resp->headers);
	strcat(buf, "\r\n"); // Empty line
	if( strlen(resp->body) != 0 )
		strcat(buf, resp->body);
	return buf;
}

struct response* pageNotFoundResponse(){
	return createResponse("HTTP/1.1", "404", "Not found");
}

char* createWebSocketKey(char* clientKey){
	char* magicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	int strLen = strlen(clientKey) + strlen(magicString);
	char* str = (char*)malloc(strLen + 1);
	strcpy (str, clientKey);
  	strcat (str, magicString);

	SHA1Context sha;
	uint8_t sha1Result[20];
	SHA1Reset(&sha);
	SHA1Input(&sha,(const unsigned char *) str, strLen);
	SHA1Result(&sha, sha1Result);

	char* base64Result = (char*)malloc(29);
	base64encode(sha1Result, 20, base64Result, 28);
	base64Result[28] = '\0';
	free(str);
	return base64Result;
}

struct response* webSocketResponse(char* clientKey, char* webSocketProtocol){
	struct response* resp = createResponse("HTTP/1.1", "101", "Switching Protocols");
	addHeaderResponse(resp, "Upgrade: websocket");
	addHeaderResponse(resp, "Connection: Upgrade");
	addHeaderResponsePair(resp,
		"Sec-WebSocket-Accept",
		createWebSocketKey(clientKey));
	addHeaderResponsePair(resp, "Sec-WebSocket-Protocol", webSocketProtocol);
	return resp;
}

void freeResponse(struct response* resp){
	free(resp->protocol);
	free(resp->code);
	free(resp->msg);
	free(resp->headers);
	free(resp->body);
	free(resp);
}

