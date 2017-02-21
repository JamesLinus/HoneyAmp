#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_PACKET 65536

typedef int socket_t;
typedef void* callback_t;

struct packet_t
{
  int size;
  struct sockaddr_in addr;
  socket_t fd;
};

struct server_t
{
  uint16_t port;
  callback_t (*callback)(struct packet_t* packet);
  socket_t fd;
};

bool server_start(struct server_t* server);

#endif
