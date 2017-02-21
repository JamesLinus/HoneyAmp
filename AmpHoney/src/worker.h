#ifndef WORKER_H
#define WORKER_H

#include <time.h>

#include "server.h"
#include "list.h"

#define MAX_THRESHOLD 1200

struct request_t
{
  struct in_addr ip_addr;
  uint16_t port;
  time_t last_seen;
};

struct worker_t
{
  uint16_t port;
  void* callback;
};

bool worker_general_callback(struct packet_t* packet, struct list_t* head);
void worker_send_bytes(struct packet_t* packet, uint16_t min, uint16_t max, void* data, size_t len);
void* worker_start_server(void* worker_param);

#endif
