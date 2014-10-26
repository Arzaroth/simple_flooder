##
## by Arzaroth Lekva
## <lekva@arzaroth.com>
##

NAME1			= tcp_flooder

GNUCC			:= no

S_EXT			:= .c
H_EXT			:= .h

HAVECLANG		:= $(wildcard $(shell bash -c 'which clang 2>&1'))

ifeq ($(GNUCC), yes)
CC			:= gcc
else
ifeq ($(HAVECLANG),)
CC			:= gcc
else
CC			:= clang
endif
endif

RM			= rm -fv

DEBUG_MODE		= no

LIB			:= -lm

override CFLAGS		+=	-I/usr/include/ \
				-pipe \
				-W \
				-Wall \
				-Wextra \

ifeq ($(DEBUG_MODE), yes)
DEBUGF			+= -g -D_D_DEBUG
else
DEBUGF			+= -D_D_NDEBUG -O2
endif

$(NAME1)_SRC_DIR	= ./src/
$(NAME1)_SRC		= $(shell find $($(NAME1)_SRC_DIR) -iname "*$(S_EXT)" -type f -print | sort)

$(NAME1)_INC_DIR 	= ./inc/
$(NAME1)_INC_H_		= $(shell find $($(NAME1)_INC_DIR) -iname "*$(H_EXT)" -type f -print | sort)
$(NAME1)_INC		= $(shell find $($(NAME1)_INC_DIR) -type d -exec echo -I {} \;)

$(NAME1)_OBJ_DIR	= ./obj/
$(NAME1)_OBJ		= $($(NAME1)_SRC:$($(NAME1)_SRC_DIR)%$(S_EXT)=$($(NAME1)_OBJ_DIR)%.o)

$(NAME1)_LIB_DIR	= ./lib/
$(NAME1)_LIB_FLG	=

$(NAME1)_FLAGS		= $($(NAME1)_INC) \
			  $(DEBUGF) \
			  $(CFLAGS)

$(NAME1):	$($(NAME1)_OBJ)
	$(CC) -o $(NAME1) $($(NAME1)_OBJ) $($(NAME1)_FLAGS) -L$($(NAME1)_LIB_DIR) $($(NAME1)_LIB_FLG) $(LIB)

$($(NAME1)_OBJ_DIR)%.o:	$($(NAME1)_SRC_DIR)%$(S_EXT) $($(NAME1)_INC_H_)
	@test -d $(@D) || mkdir -p $(@D)
	$(CC) -c -o $@ $< $($(NAME1)_FLAGS)

all:	$(NAME1)

debug:
	@make -s DEBUG_MODE=yes re

clean:
	$(RM) $($(NAME1)_OBJ)

fclean:	clean
	$(RM) $(NAME1)

re:	fclean all

.PHONY:	all clean re fclean
