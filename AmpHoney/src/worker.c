#include "main.h"
#include "worker.h"
#include "server.h"
#include "util.h"
#include "list.h"

bool worker_general_callback(struct packet_t* packet, struct list_t* head)
{
  struct request_t* req;
  bool host_exists = false;

  if(head == NULL)
  {
    return false;
  }

  #ifdef DEBUG
    debug_msg("Going through list", DEBUG_INFO);
  #endif
  FOREACH(head, item)
  {
    #ifdef DEBUG
      debug_msg("Item hoe", DEBUG_INFO);
    #endif

    if(item->data == NULL)
    {
      // banter its not even adding correctly to the list :(
      printf("yea its null lol NERFNET\nSize: %zu\n", list_size(head));
      continue;
    }
    
    if(item->data->ip_addr.s_addr == packet->addr.sin_addr.s_addr)
    {
      #ifdef DEBUG
        debug_msg("Setting request along with host_exists", DEBUG_INFO);
      #endif
      req = (struct request_t*)item->data;
      host_exists = true;
      break;
    }
  }

  #ifdef DEBUG
    debug_msg("Finished going through the list", DEBUG_INFO);
  #endif

  time_t current_time;
  time(&current_time);

  if(host_exists)
  {
    if((current_time - req->last_seen) <= MAX_THRESHOLD)
    {
      #ifdef DEBUG
        debug_msg("Host contacted more than once in a 20 minute period", DEBUG_INFO);
      #endif
      return false;
    }

    req->last_seen = current_time;
  } else {
    #ifdef DEBUG
      debug_msg("Creating new one", DEBUG_INFO);
    #endif

    req = calloc(1, sizeof(struct request_t));
    req->ip_addr.s_addr = packet->addr.sin_addr.s_addr;
    req->port = ntohs(packet->addr.sin_port);
    req->last_seen = current_time;

    list_push(head, req);
  }
  return true;
}

void worker_send_bytes(struct packet_t* packet, uint16_t min, uint16_t max, void* data, size_t len)
{
  size_t packet_size;
  unsigned char* resp_bytes;

  if(data == NULL)
  {
    packet_size = util_rand_int(min, max);
    resp_bytes = util_rand_bytes(packet_size);
  } else {
    resp_bytes = (unsigned char*)data;
    packet_size = len;
  }

  int sock_flags = 0;
  #ifdef __linux__
    sock_flags = MSG_NOSIGNAL;
  #endif

  struct sockaddr* addr_ptr = (struct sockaddr*)&packet->addr;
  if(sendto(packet->fd, resp_bytes, packet_size, sock_flags, addr_ptr, sizeof(packet->addr)) == packet_size)
  {
    #ifdef DEBUG
      debug_msg("Succesfully sent payload back", DEBUG_SUCCESS);
    #endif
  }
}

void* worker_start_server(void* worker_param)
{
  struct worker_t* worker = (struct worker_t*)worker_param;

  struct server_t* server_opts = calloc(1, sizeof(struct server_t));
  server_opts->port = worker->port;
  server_opts->callback = worker->callback;
  server_opts->fd = -1;

  free(worker);

  #ifdef DEBUG
    debug_msg("Starting the server", DEBUG_INFO);
  #endif

  if(!server_start(server_opts))
  {
    #ifdef DEBUG
      debug_msg("Could not start the server..", DEBUG_ERROR);
    #endif

    free(server_opts);
  }

  #ifdef DEBUG
    debug_msg("Our server stopped working, for some reason..", DEBUG_ERROR);
  #endif

  return NULL;
}
