/*
** -*- coding: utf-8 -*-
**
** File: simple_flooder.h
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#ifndef SIMPLE_FLOODER_H_
# define SIMPLE_FLOODER_H_

# include "circ.h"

# define BUFFSIZE (512)

typedef struct client {
    int sock;
    circbuff in;
    circbuff out;
} client;

void usage(const char *);

#endif /* !SIMPLE_FLOODER_H_ */
