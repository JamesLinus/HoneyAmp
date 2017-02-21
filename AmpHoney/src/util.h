#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>

#define ZERO_MEM(a,b) _memset(a, 0, b)

#ifdef DEBUG
typedef enum debug_level_t
{
  DEBUG_SUCCESS,
  DEBUG_INFO,
  DEBUG_ERROR
} debug_level_t;

void debug_msg(const char* message, debug_level_t type);
#endif

unsigned char* util_rand_bytes(size_t size);
uint32_t util_rand_int(uint32_t min, uint32_t max);
void _memset(void* ptr, unsigned char byte, size_t size);
size_t _strlen(const char* str);
int _strcmp(const char* str1, const char* str2);

#endif
