#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define VERSION "0.5.2"
#define ARRAY_COUNT(a) ((sizeof(a)) / sizeof(a[0]))

struct options_t
{
  const char* key;
  void* callback;
  uint16_t port;
  struct list_t* list_head;
} options_t;

#endif
