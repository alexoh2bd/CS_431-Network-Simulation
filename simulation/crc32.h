#ifndef __crc32
#define __crc32 

#include <sys/types.h>
#include <stdint.h>

uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#endif
