PROJECT_DIR 	= $(realpath ./)
export PROJECT_DIR

MAKEFLAGS := -s

.PHONY: client api utils cleanall cleanutils cleanclient cleanapi memcheck debug
.DEFAULT_GOAL := all

client:
	$(MAKE) -C client

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
	@echo "All clean now!"