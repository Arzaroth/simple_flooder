/*
** -*- coding: utf-8 -*-
**
** File: opts.c
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "opts.h"
#include "simple_flooder.h"

const opt_hook opt_hooks[] = {{'g', &add_g_option},
                              {'c', &add_c_option},
                              {'h', &add_h_option},
                              {'?', &bad_option},
                              {-1, NULL}};

const struct option longopts[] = {{"grain", 1, 0, 'g'},
                                  {"connections", 1, 0, 'c'},
                                  {"help", 0, 0, 'h'},
                                  {0, 0, 0, 0}};

void clean_opts(opt *opts) {
  (void)opts;
}

int init_opts(opt *opts, char *const av[]) {
  opts->prgname = av[0];
  opts->opt[G_OPTION].intval = G_DEFAULT;
  opts->opt[C_OPTION].intval = C_DEFAULT;
  return EXIT_SUCCESS;
}

int add_g_option(opt *opts) {
  long long arg;
  char *p;

  if ((arg = strtoll(optarg, &p, 10)) < 0 ||
      *p != 0 || arg > INT_MAX) {
    return fprintf(stderr, "%s: option expects an argument in [0..%d] range -- 'g'\n", opts->prgname, INT_MAX),
      EXIT_FAILURE;
  }
  opts->opt[G_OPTION].intval = arg;
  return EXIT_SUCCESS;
}

int add_c_option(opt *opts) {
  long long arg;
  char *p;

  if ((arg = strtoll(optarg, &p, 10)) < 0 ||
      *p != 0 || arg > INT_MAX) {
    return fprintf(stderr, "%s: option expects an argument in [0..%d] range -- 'c'\n", opts->prgname, INT_MAX),
      EXIT_FAILURE;
  }
  opts->opt[C_OPTION].intval = arg;
  return EXIT_SUCCESS;
}

int add_h_option(opt *opts) {
  printf("Simple TCP flooder\n"
         "Base launching: %s localhost 8888 15000\n"
         "Mandatory arguments to long options are mandatory for short options too.\n"
         "Options:\n"
         "  -g, --grain=NUM\t\tDisplay connections number grain.\n"
         "  -c, --connections=NUM\t\tConnections per process.\n"
         "  -h, --help\t\tShow this help and exit.\n",
         opts->prgname);
  return EXIT_FAILURE;
}

int bad_option(opt *opts) {
  usage(opts->prgname);
  return EXIT_FAILURE;
}
