/*
** -*- coding: utf-8 -*-
**
** File: opts.h
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#ifndef OPTS_H_
# define OPTS_H_

# include <getopt.h>

# define G_DEFAULT (100)
# define C_DEFAULT (1000)

enum e_option {
  G_OPTION = 0,
  C_OPTION,
  NUM_OPTIONS,
};

typedef struct {
  union {
    size_t intval;
    double dblval;
    char *str;
  } opt[NUM_OPTIONS];
  const char *prgname;
} opt;

int add_g_option(opt *);
int add_c_option(opt *);
int add_h_option(opt *);
int bad_option(opt *);

typedef struct {
  int ret;
  int (*func)(opt *);
} opt_hook;

extern const opt_hook opt_hooks[];
extern const struct option longopts[];

void clean_opts(opt *);
int init_opts(opt *, char *const av[]);

#endif /* !OPTS_H_ */
