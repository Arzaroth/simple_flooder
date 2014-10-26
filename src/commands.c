/*
** -*- coding: utf-8 -*-
**
** File: commands.c
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#include <stdarg.h>
#include "commands.h"
#include "circ.h"
#include "simple_flooder.h"

/*
** Demonstration purpose
** Add commands bellow and in commands.h file
*/
const struct command commands[] = {{"ping", &ping_cmd},
                                   {NULL, NULL}};

int ping_cmd(client *cli, ...) {
  return add_to_queue(&cli->out, "pong");
}
