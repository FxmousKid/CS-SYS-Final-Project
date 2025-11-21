SRC_DIR		= src
BUILD_DIR 	= build
INC_DIR		= include
DOCS_DIR	= docs

# source code, with root = $SRC_DIR/
SRC_FILES_NAMES = main.c
#SRC_FILES_NAMES += file.c

# Full Path to .c files
SRC_FILES = $(addprefix $(SRC_DIR)/, $(SRC_FILES_NAMES))

# .o files for compilation
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# .d files for dependencies
DEP_FILES := $(OBJ_FILES:.o=.d)

#<><><><><><><> Variables <><><><><><><><><><><><><><><><><>

NAME 	:= tadmor
CC 	:= cc
CFLAGS	:= -Wall -Wextra -Werror -I $(INC_DIR) -MMD -MP
LFLAGS	?=

MKDIR 	:= mkdir -p
RM_RF	:= rm -rf
ECHO	:= echo -e

BLUE	:= $(shell $(ECHO) "\033[34m") 
BROWN	:= $(shell $(ECHO) "\033[33m")
GREEN	:= $(shell $(ECHO) "\033[32m")
RED	:= $(shell $(ECHO) "\033[31m")
NC	:= $(shell $(ECHO) "\033[0m")

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
