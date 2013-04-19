#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MASKING_KEY_SIZE 4

struct wsFrame{
	unsigned char fin;
	unsigned char opcode;
	unsigned char mask;
	int len;
	char *maskingKey;
	char *data;
};

char* generateMaskingKey();

struct wsFrame* createTextWSFrame(char *str);

char* wsFrameToString(struct wsFrame* frame);

void freeWSFrame(struct wsFrame* frame);

#endif
