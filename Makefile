# Project directory
ORIGIN 	= $(realpath ./)
export ORIGIN

# Project sub-modules
CLIENT 	= $(ORIGIN)/client
export CLIENT

API		= $(ORIGIN)/api
export API

SERVER 	= $(ORIGIN)/server
export SERVER

UTILS 	= $(ORIGIN)/utils
export UTILS

UTIL_LIBS 	= $(UTILS)/libs
export UTIL_LIBS

API_LIBS	=$(API)/libs
export API_LIBS

# Printing
RED		= \033[1m\033[31m
GREEN	= \033[1m\033[32m
BOLD	= \033[1m
RESET	= \033[0m

# Executables
CLIENT_EXE		= $(CLIENT)/bin/client
SERVER_EXE		= $(SERVER)/bin/server

# Make flags
MAKEFLAGS += --no-print-directory
export MAKEFLAGS

# Compiler flags
CFLAGS		+= -Wall -std=c99 -D_POSIX_C_SOURCE 
export CFLAGS

INCLUDES	= -I$(ORIGIN)
export INCLUDES

DEBUG		= -DDEBUG=true -g

# Parameter list for debugging and memchecking
CLIENT_PARAMS	= -R 5

# Targets
.PHONY: all	cleanall m\
		client cleanclient 	\
		server cleanserver 	\
		api cleanapi 		\
		utils cleanutils	\
		run_client			\
		run_server			\
		client_memcheck		\
		server_memcheck		\
		debug				\
		test cleantest	    \
		
.DEFAULT_GOAL := all

# ******** Rules ********

# Compilation
client:
	$(MAKE) -C $(CLIENT)

server:
	$(MAKE) -C $(SERVER)

api:
	$(MAKE) -C $(API)

utils:
	$(MAKE) -C $(UTILS)

# Default Goal
all:
	@echo "${BOLD}Building application... ${RESET}"
	@echo "${BOLD}Building utilities... ${RESET}"
	@make utils
	@echo "${GREEN}Utilities built ${RESET}"
	@echo "${BOLD}Building API... ${RESET}"
	@make api
	@echo "${GREEN}API built ${RESET}"
	@echo "${BOLD}Building client... ${RESET}"
	@make client
	@echo "${GREEN}Client built ${RESET}"
	@echo "${BOLD}Building server... ${RESET}"
	@make server
	@echo "${GREEN}Server built ${RESET}"


# Execution
run_client:	
	$(CLIENT_EXE) $(CLIENT_PARAMS)

run_server:
	$(SERVER_EXE)

# Debugging, testing and memory checking
debug: CFLAGS += $(DEBUG)
debug: 
	@make all

test: CFLAGS += -g
test: 
	@make cleanall debug
	@cp $(CLIENT_EXE) test/
	@cp $(SERVER_EXE) test/

client_memcheck:
	@valgrind -v --track-origins=yes --leak-check=full $(CLIENT_EXE) $(CLIENT_PARAMS)

server_memcheck:
	@valgrind -v --track-origins=yes --leak-check=full $(SERVER_EXE)

# Cleaning
cleanclient:
	@cd client && make cleanall
	@echo "${GREEN}Client removed ${RESET}"

cleanserver:
	@cd server && make cleanall
	@echo "${GREEN}Server removed ${RESET}"

cleanapi:
	@cd api && make cleanall
	@echo "${GREEN}API removed ${RESET}"

cleanutils:
	@cd utils && make cleanall
	@echo "${GREEN}Utilites removed ${RESET}"

cleantest:
	@cd test && rm -rf client server logs/*.txt
	@echo "${GREEN}Test directory cleaned ${RESET}"

cleanall:
	@echo "${BOLD}Cleaning up... ${RESET}"
	@make cleantest
	@make cleanutils
	@make cleanapi
	@make cleanclient
	@make cleanserver
	@echo "${BOLD}All clean now!${RESET}"