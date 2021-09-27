# General
CC			= gcc
LD			= gcc
RM			= rm -rf

# Directories
PROJDIR		= $(realpath ../)
SRCDIR		= src
OBJDIR		= obj
BINDIR		= bin
INCDIR		= $(PROJDIR)
APIDIR 		= $(PROJDIR)/api
UTILDIR		= $(PROJDIR)/utils

# Targets
TARGET		= client

# Library targets
QUEUE 		= $(UTILDIR)/libs/libqueue.so
API 		= $(APIDIR)/libs/libfilestorage_api.so

# Linking
LINK_API 	= -Wl,-rpath,$(APIDIR)/libs
LINK_API	+= -L $(APIDIR)/libs
LINK_API	+= -lfilestorage_api

LINK_QUEUE 	= -Wl,-rpath,$(UTILDIR)/libs
LINK_QUEUE	+= -L $(UTILDIR)/libs
LINK_QUEUE	+= -lqueue
 
LINK_ALL	= $(LINK_API)
LINK_ALL	+= $(LINK_QUEUE)
# Flags
CFLAGS		:= -std=c99 
INCLUDES	= -I$(INCDIR)
DEBUG		= -DDEBUG=true -g


# Compiler Flags
ALL_CFLAGS		:= $(CFLAGS)
ALL_CFLAGS		+= -Wextra -pedantic 
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
BIN			:= $(BINDIR)/$(TARGET)
-include $(DEP)

# General rule for objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD -MP -o $@ $<

# General rule for libraries
$(LIBDIR)/%.so: $(OBJ)
	$(CC) -shared -o $@ $<

$(BIN): $(OBJ)
	$(LD) $(CFLAGS) -o $@ $^ $(LINK_ALL)

# Build Rules
.PHONY: clean memcheck debug
.DEFAULT_GOAL := all

all: setup $(BIN)
setup: dir
	
remake: clean all

dir:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

debug: CFLAGS += $(DEBUG)
debug: remake

PARAMS	:= -R 10 -W file1,file2,file3,file4 -r file1,file2,file3,file4 \
		   -l lock1,lock2 -u ulock2,ulock3 -c clean1,clean2 -t 10 -p \
		   -f ./tmp/filestorage.sk -D trash_here/ -d read_here/
memcheck:
	@make remake
	@valgrind --leak-check=full ./$(BIN) $(PARAMS)
	@make clean

clean:
	$(RM) $(OBJ) $(DEP) $(BIN)
	$(RMDIR) $(OBJDIR) $(BINDIR) 2> /dev/null; true
