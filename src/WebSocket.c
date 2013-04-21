#include "WebSocket.h"

char* generateMaskingKey(){
	char* maskingKey = (char*)malloc(MASKING_KEY_SIZE+1);
	int i;	
	for(i = 0; i < MASKING_KEY_SIZE; i++){
		maskingKey[i] = rand()%255;
	}
	return maskingKey;
}

struct wsFrame* createTextWSFrame(char *str){
	struct wsFrame* frame = (struct wsFrame*)malloc(sizeof(struct wsFrame));
	frame->fin = 1;
	frame->opcode = 0x01;
	frame->mask = 1;
	frame->len = strlen(str);
	frame->maskingKey = generateMaskingKey();
	frame->data = (char*)malloc(frame->len+1);
	strcpy(frame->data, str);
	return frame;
}

char* wsFrameToString(struct wsFrame* frame){
	
	unsigned char byte1 = 0;
	byte1 = frame->fin<<7;
	byte1 = byte1 & 0x8F;
	byte1 = byte1 | frame->opcode;

	unsigned char byte2 = 0;
	byte2 = frame->mask<<7;
	
	unsigned char dataLength = 0;
	if(frame->len < 126){
		byte2 = byte2 | frame->len;
	}else{
		if(frame->len < 65536){
			dataLength = 2;
			byte2 = byte2 | 0x7E; //126
		}else{
			dataLength = 4;
			byte2 = byte2 | 0x7F; //127
		}
		
	}
	int strLen = 2 + dataLength;
	if(frame->mask == 1)
		strLen += MASKING_KEY_SIZE;
	strLen += frame->len;

	char* str = (char*)malloc(strLen + 1);

	unsigned int index = 0;
	memcpy(str + index, &byte1, 1);

	index += 1;
	memcpy(str+index, &byte2, 1);

	index += 1;
	if(dataLength > 0){
		int netLen = htonl(frame->len);
		memcpy(str+index, (char*)&netLen, dataLength);
		index += dataLength;
	}
	if(frame->mask == 1){
		memcpy(str+index, frame->maskingKey, MASKING_KEY_SIZE);
		index += MASKING_KEY_SIZE;
		// Apply masking key
		int i = 0;
		for(i = 0; i < frame->len; i++){
			// masked-data-i = data-i XOR masking-key-(i mod 4)
			frame->data[i] = frame->data[i]^frame->maskingKey[i%MASKING_KEY_SIZE];
		}
	}
	memcpy(str+index, frame->data, frame->len);
	str[strLen] = '\0';
	return str;
}


void freeWSFrame(struct wsFrame* frame){
	if(frame->maskingKey != NULL)
		free(frame->maskingKey);
	if(frame->data != NULL)
		free(frame->data);
	free(frame);
}
