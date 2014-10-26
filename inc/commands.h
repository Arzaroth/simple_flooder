/*
** -*- coding: utf-8 -*-
**
** File: commands.h
** by Arzaroth Lekva
** arzaroth@arzaroth.com
**
*/

#ifndef COMMANDS_H_
# define COMMANDS_H_

# include "simple_flooder.h"

/*
** Demonstration purpose
** Add commands bellow and in commands.c file
*/
int ping_cmd(client *cli, ...);

struct command {
  const char *command;
  int (*func)(client *cli, ...);
};

extern const struct command commands[];

#endif /* !COMMANDS_H_ */
