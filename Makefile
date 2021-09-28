ORIGIN 	= $(realpath ./)
export ORIGIN

CLIENT_DIR 	= client/
API_DIR		= api/
SERVER_DIR 	= server/
UTILS_DIR 	= utils/

CLIENT		= $(CLIENT_DIR)/bin/client
SERVER		= $(SERVER_DIR)/bin/server

MAKEFLAGS := -s

.PHONY: client server api utils \
		cleanall cleanutils cleanclient cleanapi \
		memcheck debug basic_test
.DEFAULT_GOAL := all

client:
	$(MAKE) -C client

server:
	$(MAKE) -C server

api:
	$(MAKE) -C api

utils:
	$(MAKE) -C utils

all:
	@echo "Building application..."
	@make utils
	@echo "Built utilites"
	@make api
	@echo "Built API"
	@make client
	@echo "Built client"

basic_test:
	@make all
	@echo "Moving executables to test directory"
	@cp $(SERVER) ./test 
	@cp $(CLIENT) ./test
	@echo "Executables are ready"

memcheck:
	@make all
	@cd client && make memcheck

debug:
	@make all
	@cd client && make debug

cleanutils:
	@cd utils && make cleanall
	@echo "Utilities removed"

cleanapi:
	@cd api && make cleanall
	@echo "API removed"

cleanclient:
	@cd client && make cleanall
	@echo "Client removed"

cleanall:
	@echo "Cleaning up..."
	@make cleanutils
	@make cleanapi
	@make cleanclient
	@rm -rf $(SERVER)
	@rm -rf test/*
	@echo "All clean now!"