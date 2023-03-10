#ifndef __util
#define __util

#include <stdio.h>
#include <sys/types.h>
char *binary_to_hex(void *data, ssize_t n);
void *hex_to_binary(char *hex);


#endif
