SRC_DIR		= src
BUILD_DIR 	= build
INC_DIR		= include
DOCS_DIR	= docs

# source code, with root = $SRC_DIR/
SRC_FILES_NAMES = main.c

SRC_FILES_NAMES += parser/parse_cli.c

SRC_FILES_NAMES += utils/utils.c

SRC_FILES_NAMES += commands/list_tasks.c

SRC_FILES_NAMES += communication/fifo_api.c

# Full Path to .c files
SRC_FILES = $(addprefix $(SRC_DIR)/, $(SRC_FILES_NAMES))

# .o files for compilation
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# .d files for dependencies
DEP_FILES := $(OBJ_FILES:.o=.d)


#<><><><><><><> Variables <><><><><><><><><><><><><><><><><>

NAME 	:= tadmor
CC 	:= cc
CFLAGS	= -Wall -Wextra -Werror -I $(INC_DIR) -MMD -MP
LFLAGS	?= -lbsd

LOGFILE_PATH ?= $(PWD)/tadmor.log
CFLAGS	+= -DLOGFILE_PATH=\"$(LOGFILE_PATH)\"

MKDIR 	:= mkdir -p
RM_RF	:= rm -rf
ECHO	:= printf '%b\n'

BLUE	:= \033[34m
BROWN	:= \033[33m
GREEN	:= \033[32m
RED	:= \033[31m
NC	:= \033[0m

#<><><><><><><> Recipes <><><><><><><><><><><><><><><><><><>

# Modifying Implicit conversion rules to build in custom directory
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	@$(MKDIR) $(dir $@)
	@$(ECHO) "$(BLUE)[CMP] Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) -c $< -o $@ 

# Include dependency files automatically
-include $(DEP_FILES)

$(NAME) : $(OBJ_FILES)
	@$(ECHO) "$(BROWN)[BLD] Building $(NAME) executable...$(NC)"
	@$(CC) $^ -o $(NAME) $(LFLAGS)
	@$(ECHO) "$(GREEN)[BLD] Executable built successfully.$(NC)"

all: $(NAME)

re: distclean all

distclean : 
	@$(ECHO) "$(BROWN)[CLN] Cleaning object, dependency files, and executable...$(NC)"
	@$(RM_RF) $(BUILD_DIR) $(NAME)
	@$(ECHO) "$(GREEN)[CLN] Clean complete.$(NC)"

.DEFAULT_GOAL := all
.PHONY : all distclean re
