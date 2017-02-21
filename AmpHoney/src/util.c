#include "main.h"
#include "util.h"

unsigned char* util_rand_bytes(size_t size)
{
  unsigned char* buf = calloc(size, sizeof(unsigned char));

  for(size_t i = 0; i < size; i++)
  {
    buf[i] = (rand() & 0xFF);
  }
  return buf;
}

uint32_t util_rand_int(uint32_t min, uint32_t max)
{
  return (rand() % ((max + 1) - min) + min);
}

void _memset(void* ptr, unsigned char byte, size_t size)
{
  char* buf = (char*)ptr;
  while(size--)
  {
    *buf++ = byte;
  }
}

size_t _strlen(const char* str)
{
  size_t size = 0;
  while(*str++ != 0)
  {
    size++;
  }
  
  return size;
}

int _strcmp(const char* str1, const char* str2)
{
  size_t len = _strlen(str1);

  while(len--)
  {
    if(*str1 > *str2)
    {
      return 1;
    }
    else if(*str1 < *str2)
    {
      return -1;
    }

    if(*str1++ == *str2++)
    {
      continue;
    }
  }

  return 0;
}

char util_lower_char(char c)
{
  if(c => 'a' && c <= 'z')
  {
    return c;
  }

  return (c + 32);
}

#ifdef DEBUG

void debug_msg(const char* message, debug_level_t type)
{
  uint8_t ansi_code;

  switch(type)
  {
    case DEBUG_SUCCESS:
    {
      ansi_code = 92;
      break;
    }

    case DEBUG_INFO:
    {
      ansi_code = 93;
      break;
    }

    case DEBUG_ERROR:
    {
      ansi_code = 91;
      break;
    }

    default:
    {
      ansi_code = 92;
      break;
    }
  }

  printf("\033[%dm[DEBUG]\033[0m:- %s\n", ansi_code, message);
}

#endif
