#### Main Makefile that uses deeper makefiles

TADMOR_DIR 	:= code/tadmor
ERRAID_DIR 	:= code/erraid

MKDIR 	:= mkdir -p
RM_RF	:= rm -rf
ECHO	:= echo -e

BLUE	:= $(shell $(ECHO) "\033[34m") 
BROWN	:= $(shell $(ECHO) "\033[33m")
GREEN	:= $(shell $(ECHO) "\033[32m")
RED	:= $(shell $(ECHO) "\033[31m")
NC	:= $(shell $(ECHO) "\033[0m")

$(TADMOR_DIR)/tadmor:
	@$(ECHO) "$(BLUE)[BLD] Entering $(TADMOR_DIR)/ Makefile...$(NC)"
	@$(MAKE) --no-print-directory -C $(TADMOR_DIR)
	@mv $(TADMOR_DIR)/tadmor ./
	@$(ECHO) "$(GREEN)[BLD] Leaving $(TADMOR_DIR)/ Makefile...$(NC)"

$(ERRAID_DIR)/erraid:
	@$(ECHO) "$(BLUE)[BLD] Entering $(ERRAID_DIR)/ Makefile...$(NC)"
	@$(MAKE) --no-print-directory -C $(ERRAID_DIR)
	@mv $(ERRAID_DIR)/erraid ./
	@$(ECHO) "$(GREEN)[BLD] Leaving $(ERRAID_DIR)/ Makefile...$(NC)"

all: $(TADMOR_DIR)/tadmor $(ERRAID_DIR)/erraid
	@echo
	@$(ECHO) "$(GREEN)[ALL] All sub-makefiles built successfully$(NC)"

erraid-distclean:
	@$(MAKE) --no-print-directory -s -C $(ERRAID_DIR) distclean

tadmor-distclean:
	@$(MAKE) --no-print-directory -s -C $(TADMOR_DIR) distclean

erraid: erraid-distclean $(BIN_DIR) $(ERRAID_DIR)/erraid

tadmor: tadmor-distclean $(BIN_DIR) $(TADMOR_DIR)/tadmor

distclean:
	@$(ECHO) "$(BROWN)[CLN] Cleaning all sub-makefiles...$(NC)"
	@$(MAKE) --no-print-directory -s -C $(TADMOR_DIR) distclean > /dev/null
	@$(MAKE) --no-print-directory -s -C $(ERRAID_DIR) distclean > /dev/null
	@rm tadmor erraid 2> /dev/null || true
	@$(ECHO) "$(GREEN)[CLN] All sub-makefiles cleaned.$(NC)"

re: distclean all

.DEFAULT_GOAL := all
.PHONY : all distclean re erraid tadmor
