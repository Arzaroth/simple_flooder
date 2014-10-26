/*
** -*- coding: utf-8 -*-
**
** File: simple_flooder.c
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <getopt.h>

#include "opts.h"
#include "circ.h"
#include "commands.h"
#include "simple_flooder.h"

/*
** Do your own stuff here
*/
int parse_command(char *cmd, void *res) {
  char *saveptr;

  *(char **)res = strtok_r(cmd, " ", &saveptr);
  return EXIT_SUCCESS;
}

int handle_command(client *cli) {
  char *fnd, *cmd;
  int i;

  for (fnd = strstr(cli->in.buff, "\n");
       fnd != NULL;
       fnd = cli->in.buff == NULL ? NULL : strstr(cli->in.buff, "\n")) {
    fnd[0] = 0;
    if (parse_command(cli->in.buff, &cmd) == EXIT_SUCCESS) {
      for (i = 0; commands[i].command != NULL; ++i) {
        if (!strcmp(commands[i].command, cmd)) {
          commands[i].func(cli); /* Add params here */
          break;
        }
      }
    }
    nooth_circ(&cli->in, (void *)fnd - (void *)cli->in.buff, "\n");
  }
  return EXIT_SUCCESS;
}

int handle_output(client *cli) {
  ssize_t ret;

  ret = send(cli->sock, cli->out.buff, strlen(cli->out.buff), 0);
  if (ret == -1 && errno == EINTR) {
    return EXIT_SUCCESS;
  }
  if (ret <= 0) {
    return EXIT_FAILURE;
  }
  if ((size_t)ret == strlen(cli->out.buff)) {
    clean_circular(&cli->out);
  } else {
    cli->out.buff += ret;
  }
  return EXIT_SUCCESS;
}

int handle_input(client *cli) {
  char buff[BUFFSIZE];
  ssize_t ret;

  bzero(buff, BUFFSIZE);
  ret = recv(cli->sock, buff, BUFFSIZE - 1, 0);
  if (ret == -1 && errno == EINTR) {
    return EXIT_SUCCESS;
  }
  if (ret <= 0) {
    return EXIT_FAILURE;
  }
  buff[ret] = 0;
  if (add_to_queue(&cli->in, buff) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int close_clients(client *cli, int where) {
  int i;

  for (i = 0; i < where; ++i) {
    close(cli[i].sock);
    clean_circular(&cli[i].in);
    clean_circular(&cli[i].out);
  }
  free(cli);
  return EXIT_FAILURE;
}

int conn_sock(struct addrinfo *res) {
  struct addrinfo *rp;
  int s;

  for (rp = res; rp != NULL; rp = rp->ai_next) {
    if ((s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) < 0) {
      return -1;
    }
    if (connect(s, rp->ai_addr, rp->ai_addrlen) == -1) {
      return -1;
    }
    return s;
  }
  return -1;
}

int open_clients(client *cli, opt *opts, const char *host, const char *port, int num) {
  struct addrinfo hints, *res;
  int maxfd = 0, sfd, i;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  if ((sfd = getaddrinfo(host, port, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sfd));
    return -1;
  }
  for (i = 0; i < num; ++i) {
    if (i % opts->opt[G_OPTION].intval == 0) {
      printf("opening client n.%d\n", i + 1);
    }
    if ((cli[i].sock = conn_sock(res)) == -1) {
      close_clients(cli, i);
      maxfd = -1;
      goto end;
    }
    maxfd = cli[i].sock > maxfd ? cli[i].sock : maxfd;
  }
 end:
  freeaddrinfo(res);
  return maxfd;
}

int loop(opt *opts, const char *host, const char *port, int num) {
  client *s;
  int j, maxfd;
  fd_set rfds, wfds;

  if ((s = calloc(num, sizeof(*s))) == NULL)
    return fprintf(stderr, "calloc: %m\n"),
      EXIT_FAILURE;
  if ((maxfd = open_clients(s, opts, host, port, num)) == -1)
    return EXIT_FAILURE;
  printf("clients opened\nIdle mode\n");
  while (1) {
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    for (j = 0; j < num; ++j) {
      FD_SET(s[j].sock, &rfds);
      if (s[j].out.buff != NULL) {
        FD_SET(s[j].sock, &wfds);
      }
    }
    if (select(maxfd + 1, &rfds, &wfds, NULL, NULL) == -1) {
      if (errno != EINTR) {
        return close_clients(s, num);
      }
    } else {
      for (j = 0; j < num; ++j) {
        if (FD_ISSET(s[j].sock, &rfds)) {
          if (handle_input(&s[j]) == EXIT_FAILURE)
            return close_clients(s, num);
        }
        if (FD_ISSET(s[j].sock, &wfds)) {
          if (handle_input(&s[j]) == EXIT_FAILURE)
            return close_clients(s, num);
        }
        if (handle_command(&s[j]) == EXIT_FAILURE)
          return close_clients(s, num);
      }
    }
  }
  return close_clients(s, num),
    EXIT_SUCCESS;
}

void usage(const char *prgname) {
  fprintf(stderr, "usage: %s [-g <grain>] [-c <connections_per_process>] <host> <port> <num of clients>\n", prgname);
}

int main(int ac,
         char *const av[],
         char *const envp[]) {
  int tnum, i, j, fnum, num, status, ret, argcount;
  pid_t tmp, *pids;
  opt opts;

  (void)envp;
  if (init_opts(&opts, av) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  while ((ret = getopt_long(ac, av, "g:c:h", longopts, (int[]){0})) != -1) {
    for (i = 0; opt_hooks[i].ret != -1 && opt_hooks[i].ret != ret; ++i);
    if (opt_hooks[i].ret != -1) {
      if (opt_hooks[i].func(&opts) == EXIT_FAILURE) {
        return clean_opts(&opts), EXIT_FAILURE;
      }
    }
  }
  argcount = ac - optind;
  if (argcount < 3) {
    usage(av[0]);
    return EXIT_FAILURE;
  }
  tnum = strtol(av[optind + 2], NULL, 10);
  if (tnum <= 0) {
    return fprintf(stderr, "%s: number of clients must be a strictly positive integer\n", av[0]),
      EXIT_FAILURE;
  }
  fnum = (int)ceil(tnum / (float)opts.opt[C_OPTION].intval);
  if ((pids = calloc(fnum, sizeof(*pids))) == NULL) {
    return fprintf(stderr, "calloc: %m\n"),
      EXIT_FAILURE;
  }
  for (i = 0; i < fnum; ++i) {
    num = (tnum - i * opts.opt[C_OPTION].intval < opts.opt[C_OPTION].intval ?
           tnum - i * opts.opt[C_OPTION].intval : opts.opt[C_OPTION].intval);
    if ((tmp = fork()) == 0) {
      exit(loop(&opts, av[optind], av[optind + 1], num));
    } else if (tmp > 0) {
      pids[i] = tmp;
    } else {
      for (j = 0; j < i; ++j) {
        kill(pids[j], SIGTERM);
      }
      free(pids);
      return EXIT_FAILURE;
    }
  }
  while (wait(&status) != -1 && errno != ECHILD);
  free(pids);
  return EXIT_SUCCESS;
}
