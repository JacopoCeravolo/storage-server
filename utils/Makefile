# General
CC			= gcc
LD			= gcc
RM			= rm -rf

# Directories
ifndef ORIGIN
ORIGIN		= $(realpath ../)
endif
SRCDIR		= src
OBJDIR		= obj
INCDIR		= include
ifndef UTIL_LIBS
UTIL_LIBS		= libs
endif

# Library targets
QUEUE 		= $(UTIL_LIBS)/libqueue.so
LIST		= $(UTIL_LIBS)/liblist.so
PROTOCOL 	= $(UTIL_LIBS)/libprotocol.so
HASHTABLE 	= $(UTIL_LIBS)/libhashtable.so

# Flags
ifndef INCLUDES
INCLUDES	= -I$(INCDIR)
else 
INCLUDES 	+= -I$(INCDIR)
endif

# Compiler Flags
ALL_CFLAGS		:= $(CFLAGS)
ALL_CFLAGS		+= -Wall -Wextra -pedantic -ansi
ALL_CFLAGS		+= -fno-strict-aliasing
ALL_CFLAGS		+= -Wuninitialized -Winit-self -Wfloat-equal
ALL_CFLAGS		+= -Wundef -Wshadow -Wc++-compat -Wcast-qual -Wcast-align
ALL_CFLAGS		+= -Wconversion -Wsign-conversion -Wjump-misses-init
ALL_CFLAGS		+= -Wno-multichar -Wpacked -Wstrict-overflow -Wvla
ALL_CFLAGS		+= -Wformat -Wno-format-zero-length -Wstrict-prototypes
ALL_CFLAGS		+= -Wno-unknown-warning-option

# Source, Binaries, Dependencies
SRC			:= $(shell find $(SRCDIR) -type f -name '*.c')
OBJ			:= $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRC:.c=.o))
DEP			:= $(OBJ:.o=.d)
-include $(DEP)

# General rule for objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD -MP -fPIC -o $@ $<

# General rule for libraries
$(LIBDIR)/%.so: $(OBJDIR)%.o
	$(RM) $@
	$(CC) -shared -o $@ $<

.PHONY: queue clean cleanall protocol test_protocol
.DEFAULT_GOAL := all

all: 
	@make queue
	@make protocol
	@make list
	@make hashtable

queue: 
	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD -MP -fPIC -o $(OBJDIR)/queue.o $(SRCDIR)/queue.c
	$(RM) $(QUEUE)
	$(CC) -shared -o $(QUEUE) $(OBJDIR)/queue.o

list: 
	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD -MP -fPIC -o $(OBJDIR)/list.o $(SRCDIR)/list.c
	$(RM) $(LIST)
	$(CC) -shared -o $(LIST) $(OBJDIR)/list.o

protocol: 
	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD -MP -fPIC -o $(OBJDIR)/protocol.o $(SRCDIR)/protocol.c
	$(RM) $(PROTOCOL)
	$(CC) -shared -o $(PROTOCOL) $(OBJDIR)/protocol.o

hashtable: 
	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD -MP -fPIC -o $(OBJDIR)/icl_hash.o $(SRCDIR)/icl_hash.c
	$(RM) $(HASHTABLE)
	$(CC) -shared -o $(HASHTABLE) $(OBJDIR)/icl_hash.o


test_protocol: $(PROTOCOL)
	$(CC) -I$(INCDIR) -o server server.c -Wl,-rpath=./$(LIBDIR)/ -L ./$(LIBDIR)/ -lqueue
	$(CC) -I$(INCDIR) -o client client.c -Wl,-rpath=./$(LIBDIR) -L ./$(LIBDIR)/ -lqueue
	

clean:
	$(RM) $(OBJ) $(DEP) 

cleanall:
	$(RM) $(OBJ) $(DEP) $(QUEUE) $(PROTOCOL) $(LIST) $(HASHTABLE)