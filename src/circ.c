/*
** -*- coding: utf-8 -*-
**
** File: circ.c
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "circ.h"

void clean_circular(circbuff *circ)
{
  free(circ->head);
  circ->head = NULL;
  circ->buff = circ->head;
  circ->size = 0;
}

int copy_circular(circbuff *dest, const circbuff *src)
{
  memset(dest, 0, sizeof(*dest));
  if (src->size == 0) {
    return EXIT_SUCCESS;
  }
  if ((dest->head = calloc(sizeof(*(dest->head)),
                           src->size + 1)) == NULL) {
    return fprintf(stderr, "calloc: %m\n"), EXIT_FAILURE;
  }
  dest->buff = dest->head;
  dest->size = src->size;
  strcat(dest->buff, src->buff);
  return EXIT_SUCCESS;
}

int add_to_queue(circbuff *cli, const char *buff)
{
  char *ret;
  size_t dec;

  if (cli->head == NULL) {
      cli->head = malloc(sizeof(*cli->head) * (strlen(buff) + 1));
      if (cli->head == NULL) {
        return EXIT_FAILURE;
      }
      cli->buff = cli->head;
      cli->head[0] = 0;
      cli->size = strlen(buff);
  } else {
      ret = realloc(cli->head, sizeof(*cli->head)
                    * (strlen(buff) + cli->size + 2));
      if (ret == NULL) {
        return EXIT_FAILURE;
      }
      dec = cli->buff - cli->head;
      cli->head = ret;
      cli->buff = cli->head + dec;
      cli->size += strlen(buff);
  }
  strcat(cli->buff, buff);
  return EXIT_SUCCESS;
}

void nooth_circ(circbuff *circ, size_t find, char *pattern)
{
  circ->buff += find + strlen(pattern);
  if (strlen(circ->buff) == 0) {
    clean_circular(circ);
  }
}
