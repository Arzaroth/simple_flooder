/*
** -*- coding: utf-8 -*-
**
** File: circ.h
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#ifndef CIRC_H_
# define CIRC_H_

# include <string.h>

typedef struct {
  char *buff;
  char *head;
  size_t size;
} circbuff;

void clean_circular(circbuff *);
int copy_circular(circbuff *, const circbuff *);
int add_to_queue(circbuff *, const char *);
void nooth_circ(circbuff *, size_t, char *);

#endif /* !CIRC_H_ */
