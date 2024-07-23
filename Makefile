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
CFLAGS		= -Wall -Werror -Wextra -std=c++98 -g
DEBUG_FLAG	= -D DEBUG

################################# Webserv src ################################

DEBUG		= debug/debug.cpp
MINILIB		= minilib/Minilib.cpp
START		= start/Parser.cpp \
			start/Parser-Utils.cpp \
			start/Server.cpp \
			start/Setters.cpp \
			start/StartServer.cpp

MAND_SRCS	= main.cpp $(DEBUG) $(MINILIB) $(START)
SRC_DIR		= ./src
SRC_FILES = $(addprefix ./src/, $(MAND_SRCS))
TEST_DIR	= $(SRC_DIR)/tests
TEST_SRCS	= $(shell find $(TEST_DIR) -name '*.cpp')
SRCS_COMMON = $(filter-out main.cpp, $(MAND_SRCS))

################################# Webserv objects ###########################

OBJS_DIR	= ./objs
OBJS 		= $(patsubst $(SRC_DIR)/%.cpp, $(OBJS_DIR)/%.o, $(SRC_FILES))
TEST_OBJS	= $(patsubst $(TEST_DIR)/%.cpp, $(OBJS_DIR)/%.o, $(TEST_SRCS))
OBJS_COMMON	= $(patsubst $(SRC_DIR)/%.cpp, $(OBJS_DIR)/%.o, $(SRC_COMMON))

################################# Progress ##################################

TOTAL_FILES		= $(words $(SRC_FILES))
CURRENT_FILES	= 0

define print_progress
	$(eval CURRENT_FILES=$(shell echo $$(($(CURRENT_FILES)+1))))
	@echo -n "\r$(MAG_B)Progress: $(MAGENT)$(CURRENT_FILES) / $(TOTAL_FILES) [$$((($(CURRENT_FILES) * 100) / $(TOTAL_FILES)))%] $(RESET) : $(BLUE)$(1)$(RESET) "
endef

################################# Rules #####################################

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo ""
	@echo "$(GREEN)$(NAME) created$(RESET)"

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CPP) $(CFLAGS) -c $< -o $@
	$(call print_progress, $(BLUE_B)Compiling:$(RESET) $<)

$(OBJS_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CPP) $(CFLAGS) -c $< -o $@
	@echo "$(BLUE_B)Compiling:$(RESET) $<"

debug: CFLAGS += $(DEBUG_FLAG)
debug: clean $(NAME)
	@echo "$(GREEN)$(NAME) compiled with debug flag$(RESET)"

test: $(TEST_OBJS) $(OBJS_COMMON)
	@$(CPP) $(CFLAGS) $(TEST_OBJS) $(OBJS_COMMON) -o $(TEST_N)
	@echo "$(GREEN)$(TEST_N) created$(RESET)"

fclean: clean
	@$(RM) $(NAME) $(TEST_N)

clean:
	@$(RM) -rf $(OBJS_DIR)

re: fclean all

.PHONY: all debug test clean fclean