#include "vsstring.h"

char* strmerge(char* str1, char* str2){
	char* str = (char*)malloc(strlen(str1) + strlen(str2) + 1);
	strcpy(str, str1);
	strcat(str, str2);
	return str;
}

char* intToStr(int num){
	char* str = (char*)malloc(12);
	sprintf(str, "%d", num);
	return str;
}
