#ifndef BASE64_H
#define BASE64_H

#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include <string.h>
 
int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize);

#endif
