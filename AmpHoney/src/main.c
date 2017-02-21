#include "main.h"
#include "server.h"
#include "util.h"
#include "list.h"
#include "worker.h"

static callback_t dns_callback(struct packet_t* packet);
static callback_t ntp_callback(struct packet_t* packet);
static callback_t snmp_callback(struct packet_t* packet);
static callback_t chargen_callback(struct packet_t* packet);
static void main_show_usage(void);
static void main_cleanup_garbage(void);
static void main_print_packet(struct packet_t* packet);
static void main_setup_signals(void);
static void main_handle_signals(int s);

struct options_t honey_options[] = {
 {
   "--dns", dns_callback, 53, NULL
 },
 {
   "--ntp", ntp_callback, 123, NULL
 },
 {
   "--snmp", snmp_callback, 161, NULL
 },
 {
   "--chargen", chargen_callback, 19, NULL
 }
};

int main(int argc, char* argv[])
{
  #ifdef DEBUG
    debug_msg("Started honeypot routine", DEBUG_INFO);
  #endif

  uid_t current_uid = getuid();
  if(current_uid != 0)
  {
    printf("Please run as root (sudo %s)\n", argv[0]);
    return EXIT_FAILURE;
  }

  if(argc < 2)
  {
    main_show_usage();
    return EXIT_SUCCESS;
  }

  if(_strcmp(argv[1], "--help") == 0)
  {
    main_show_usage();
    return EXIT_SUCCESS;
  }

  main_setup_signals();

  for(int i = 0; i < ARRAY_COUNT(honey_options); i++)
  {
    honey_options[i].list_head = calloc(1, sizeof(struct list_t));
  }

  pthread_t threads[ARRAY_COUNT(honey_options)];
  uint8_t valid_args = 0;

  for(int i = 0; i < argc; i++)
  {
    for(int j = 0; j < ARRAY_COUNT(honey_options); j++)
    {
      if(_strcmp(argv[i], honey_options[j].key) == 0)
      {
        struct worker_t* worker = calloc(1, sizeof(struct worker_t));
        worker->callback = honey_options[j].callback;
        worker->port = honey_options[j].port;

        if(pthread_create(&threads[i], NULL, worker_start_server, (void*)worker) == 0)
        {
          #ifdef DEBUG
            debug_msg("Created worker thread OK.", DEBUG_INFO);
          #endif

          valid_args++;
        } else {
          #ifdef DEBUG
            debug_msg("Could not create the worker thread.", DEBUG_ERROR);
          #endif

          free(worker);
        }
      }
    }
  }

  if(valid_args == 0)
  {
    main_show_usage();
    return EXIT_SUCCESS;
  }

  for(int i = 0; i < ARRAY_COUNT(honey_options); i++)
  {
    pthread_join(threads[i], NULL);
  }

  #ifdef DEBUG
    debug_msg("Cleanly exit, although the server should never stop..", DEBUG_INFO);
  #endif

  main_cleanup_garbage();

  return EXIT_FAILURE;
}

static void main_setup_signals(void)
{
  const int catch_signals[] = {
    SIGINT,
    SIGTERM,
    SIGABRT
  };

  for(int i = 0; i < ARRAY_COUNT(catch_signals); i++)
  {
    if(signal(catch_signals[i], main_handle_signals) == SIG_ERR)
    {
      #ifdef DEBUG
        debug_msg("Could not catch! Something already attached?", DEBUG_ERROR);
      #endif
    }
  }
}

static void main_show_usage(void)
{
  printf(
    "\033[93m"
    "                                  ...vvvv)))))).\n"
    "       /~~\\               ,,,c(((((((((((((((((/\n"
    "      /~~c \\.         .vv)))))))))))))))))))\\``\n"
    "          G_G__   ,,(((KKKK//////////////'\n"
    "        ,Z~__ '@,gW@@AKXX~MW,gmmmz==m_.\n"
    "       iP,dW@!,A@@@@@@@@@@@@@@@A` ,W@@A\\c\n"
    "       ]b_.__zf !P~@@@@@*P~b.~+=m@@@*~ g@Ws.\n"
    "          ~`    ,2W2m. '\\[ ['~~c'M7 _gW@@A`'s\n"
    "            v=XX)====Y-  [ [    \\c/*@@@*~ g@@i\n"
    "           /v~           !.!.     '\\c7+sg@@@@@s.\n"
    "          //              'c'c       '\\c7*X7~~~~\n"
    "         ]/                 ~=Xm_       '~=(Gm_.\n\n"
    "\033[0m"
    "==================================\n"
    "\033[96mHoneyAmp\033[0m (%s) - https://github.com/XXX/honeyamp\n\n"
    "\033[92mHoneyAmp allows you to pose as a vulnerable device running a amplifiable DDoS vector.\n"
    "Due to the nature of amplification attacks, you will be contacted by an IP address posing as the target\n"
    "\033[0m"
    "==================================\n"
    "+----------------------+-----------+----------+\n"
    "| Amplification vector | Argument  | Protocol |\n"
    "+----------------------+-----------+----------+\n"
    "| DNS                  | --dns     | UDP      |\n"
    "| CHARGEN              | --chargen | UDP      |\n"
    "| SNMP                 | --snmp    | UDP      |\n"
    "| NTP                  | --ntp     | UDP      |\n"
    "+----------------------+-----------+----------+\n\n"
    "Type --help for help\n\n",
    VERSION
  );
}

static void main_handle_signals(int sig)
{
  switch(sig)
  {
    case SIGINT:
    {
      printf("Are you sure you want to exit? [y/n]");
      char user_option = getchar();
      if(user_option == 'y')
      {
        main_cleanup_garbage();
        exit(EXIT_SUCCESS);
      }
      break;
    }

    case SIGTERM:
    {
      printf("We were teminated by another process..\n");
      main_cleanup_garbage();
      exit(EXIT_SUCCESS);
      break;
    }

    case SIGABRT:
    {
      printf("Abnormaly aborted for some reason..\n");
      main_cleanup_garbage();
      exit(EXIT_SUCCESS);
      break;
    }
  }
}

static void main_cleanup_garbage(void)
{
  #ifdef DEBUG
    debug_msg("Binman inbound..\n", DEBUG_INFO);
  #endif
  for(int i = 0; i < ARRAY_COUNT(honey_options); i++)
  {
    struct list_t* option_head = honey_options[i].list_head;
    FOREACH (option_head, node)
    {
      if(node == NULL)
      {
        continue;
      }
      #ifdef DEBUG
        printf("[1] Sweeping up rubbish at %p\n", honey_options[i].list_head);
      #endif
      free(node);
    }

    if (honey_options[i].list_head != NULL)
    {
      #ifdef DEBUG
        printf("[2] Sweeping up rubbish at %p\n", honey_options[i].list_head);
      #endif
      free(honey_options[i].list_head);
    }
  }
}

static void main_print_packet(struct packet_t* packet)
{
  printf(
    "IPv4 address: %s\n"
    "Port: %d\n"
    "Packet size: %d\n",
    inet_ntoa(packet->addr.sin_addr),
    htons(packet->addr.sin_port),
    packet->size
  );
}

static struct list_t* main_get_head(callback_t callback)
{
  for(int i = 0; i < ARRAY_COUNT(honey_options); i++)
  {
    if(honey_options[i].callback == callback)
    {
      return honey_options[i].list_head;
    }
  }
  return NULL;
}

static callback_t chargen_callback(struct packet_t* packet)
{
  if(worker_general_callback(packet, (main_get_head(chargen_callback))))
  {
    worker_send_bytes(packet, 300, 1500, NULL, 0);
  }

  free(packet);
  return NULL;
}

static callback_t snmp_callback(struct packet_t* packet)
{
  if(worker_general_callback(packet, (main_get_head(snmp_callback))))
  {
    worker_send_bytes(packet, 400, 1500, NULL, 0);
  }

  free(packet);
  return NULL;
}

static callback_t ntp_callback(struct packet_t* packet)
{
  if(worker_general_callback(packet, (main_get_head(ntp_callback))))
  {
    worker_send_bytes(packet, 200, 1000, NULL, 0);
  }

  free(packet);
  return NULL;
}

static callback_t dns_callback(struct packet_t* packet)
{
  if(worker_general_callback(packet, (main_get_head(dns_callback))))
  {
    #ifdef DEBUG
      debug_msg("Sending packets", DEBUG_INFO);
    #endif
    worker_send_bytes(packet, 3000, 5000, NULL, 0);
  }

  free(packet);
  return NULL;
}
