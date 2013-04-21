#ifndef WEBSOCKET_H
#define WEBSOCKET_H
/*
* Author: Sergey Zolotykh
*/

// Include
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MASKING_KEY_SIZE 4

#define WS_OPCODE_CONTINUATION 0x0
#define WS_OPCODE_TEXT 0x1
#define WS_OPCODE_BINARY 0x2
#define WS_OPCODE_CLOSE 0x8
#define WS_OPCODE_PING 0x9
#define WS_OPCODE_PONG 0xA

// Web socket frame structure
struct wsFrame{
	unsigned char fin;
	unsigned char opcode;
	unsigned char mask;
	int len;
	char *maskingKey;
	char *data;
};

// Functions

/* Generate masking key
* Return:
*	Pointer on new masking key string
*/
char* generateMaskingKey();


/* Create new web socket frame
* Parameters:
*	data - Pointer on data array
*	len - Length of data
*	opcome - Frame type
* Return:
*	Pointer on new web socket frame
*/
struct wsFrame* createWSFrame(char *data, int len, unsigned char opcode);


/* Create new text web socket frame
* Parameters:
*	str - Pointer on string
* Return:
*	Pointer on new web socket frame
*/
struct wsFrame* createTextWSFrame(char *str);


/* Make string from web socket frame
* Parameters:
*	frame - Pointer on web socket frame
* Return:
*	Pointer on new string
*/
char* wsFrameToString(struct wsFrame* frame);


/* Free web socket frame
* Parameters:
*	frame - Pointer on web socket frame
*/
void freeWSFrame(struct wsFrame* frame);

#endif
