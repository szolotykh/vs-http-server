#ifndef WEBSOCKET_H
#define WEBSOCKET_H

struct wsFrame{
	unsigned char fin;
	unsigned char opcode;
	unsigned char mask;
	int len;
	char *maskingKey;
	char *data;
};

#endif
