# Project directory
ORIGIN 	= $(realpath ./)
export ORIGIN

# Printing
RED		= \033[1m\033[31m
GREEN	= \033[1m\033[32m
BOLD	= \033[1m
RESET	= \033[0m

# Project sub-modules
CLIENT_DIR 	= client
API_DIR		= api
SERVER_DIR 	= server
UTILS_DIR 	= utils

# Executables
CLIENT		= $(CLIENT_DIR)/bin/client
SERVER		= $(SERVER_DIR)/bin/server

# Make flags
MAKEFLAGS += --no-print-directory
export MAKEFLAGS

# Compiler flags
CFLAGS		+= -Wall -std=c99
export CFLAGS
DEBUG		= -DDEBUG=true -g

# Parameter list for debugging and memchecking
PARAMS	= -R 10 -W file1,file2,file3,file4 -r file1,file2,file3,file4 \
		   		-l lock1,lock2 -u ulock2,ulock3 -c clean1,clean2 -t 10 -p \
				-D trash_here/ -d read_here/

# Targets
.PHONY: client server api utils \
		cleanall cleanutils cleanclient cleanapi \
		run_server run_client \
		memcheck debug
.DEFAULT_GOAL := all

# Rules
client:
	$(MAKE) -C client

server:
	$(MAKE) -C server

api:
	$(MAKE) -C api

utils:
	$(MAKE) -C utils

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

debug: CFLAGS += $(DEBUG)
debug: 
	@make all

run_server:
	./$(SERVER)

run_client:
	./$(CLIENT) $(PARAMS)

client_memcheck:
	@valgrind -v --track-origins=yes --leak-check=full ./$(CLIENT) $(PARAMS)
server_memcheck:
	@valgrind -v --track-origins=yes --leak-check=full ./$(SERVER)
cleanutils:
	@cd utils && make cleanall
	@echo "${GREEN}Utilites removed ${RESET}"

cleanapi:
	@cd api && make cleanall
	@echo "${GREEN}API removed ${RESET}"

cleanclient:
	@cd client && make cleanall
	@echo "${GREEN}Client removed ${RESET}"

cleanserver:
	@cd server && make cleanall
	@echo "${GREEN}Server removed ${RESET}"

cleanall:
	@echo "${BOLD}Cleaning up... ${RESET}"
	@make cleanutils
	@make cleanapi
	@make cleanclient
	@make cleanserver
	@echo "${BOLD}All clean now!${RESET}"