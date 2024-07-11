#------------------------- Makefile for Webserv ---------------------------# 
################################ Colors ####################################  

BLUE 	= \033[0;34m
MAGENT 	= \033[0;35m
RED 	= \033[1;31m
GREEN 	= \033[1;32m
YELLOW 	= \033[1;33m
BLUE_B 	= \033[1;34m
MAG_B	= \033[1;35m
WHITE 	= \033[1;37m
RESET	= \033[0m

################################# Project ###################################

NAME 	= webserv
TEST_N 	= webservTESTER
HEADER	= ./include/

################################# Flags #####################################

CPP			= c++
CFLAGS		= -Wall -Werror -Wextra -g3



MAND_SRC	= main.cpp
SRC			= $(addprefix ./src/, $(MAND_SRC))
TEST_SRC	= $(shell find $(TEST_DIR) -name '*cpp')


################################# Webserv objects ###########################

OBJS		= $(addprefix $(OBJS_DIR)/, $(MAND_SRC:.cpp=.o))
TEST_OBJS	= $(addprefix $(OBJS_DIR)/, $(notdir $(TEST_SRCS:.c=.o)))
OBJS_DIR	= ./objs

SRC_TESTCOMMON	= $(filter-out main.c, $(MAND_SRC))

################################# Rules #####################################

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) created$(RESET)"

################################# Flags #####################################

$(OBJS_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@
	$(call print_progress, $(BLUE_B)Compiling:$(RESET) $<)

$(TEST_OBJS)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@
	$(call print_progress, $(BLUE_B)Compiling:$(RESET) $<)

test: CFLAGS += TEST
test: (OBJS)

