#include "main.h"
#include "server.h"
#include "util.h"

static bool server_init(struct server_t* server)
{
  int enable_flag = 1;
  struct sockaddr_in addr;
  ZERO_MEM(&addr, sizeof(struct sockaddr_in));

  server->fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(server->fd < 0)
  {
    #ifdef DEBUG
      debug_msg("Could not create socket..", DEBUG_ERROR);
    #endif

    return false;
  }

  #ifdef __FreeBSD__
    setsockopt(server->fd, SOL_SOCKET, SO_NOSIGPIPE, (void*)&enable_flag, sizeof(int));
  #endif
  setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, (void*)&enable_flag, sizeof(int));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(server->port);

  if(bind(server->fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
  {
    #ifdef DEBUG
      debug_msg("Failed to bind to port..", DEBUG_ERROR);
    #endif

    return false;
  }
  return true;
}

bool server_start(struct server_t* server)
{
  if(server_init(server))
  {
    while(true)
    {
      char buf[MAX_PACKET];
      int numb_bytes;
      struct sockaddr_in remote_addr;
      socklen_t remote_len = (sizeof(struct sockaddr_in));

      numb_bytes = recvfrom(server->fd, buf, MAX_PACKET, MSG_WAITALL, (struct sockaddr*)&remote_addr, &remote_len);
      if(numb_bytes > 0)
      {
        struct packet_t* p = calloc(1, sizeof(struct packet_t));
        p->size = numb_bytes;
        p->addr = remote_addr;
        p->fd = server->fd;

        server->callback(p);
      }
    }
  }
  return false;
}
