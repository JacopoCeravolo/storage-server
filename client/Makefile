# General
CC			= gcc
LD			= gcc
RM			= rm -rf

# Directories
ifndef ORIGIN
ORIGIN		= $(realpath ../)
endif

ifndef UTILS
UTILS 		= $(ORIGIN)/utils
endif

ifndef API	
API			= $(ORIGIN)/api
endif

ifndef UTIL_LIBS
UTIL_LIBS	:= $(UTILS)/libs
endif

ifndef API_LIBS
API_LIBS	:= $(API)/libs
endif

ifndef INCDIR
INCDIR		= $(ORIGIN)
endif

SRCDIR		= src
OBJDIR		= obj
BINDIR		= bin
# APIDIR		= $(ORIGIN)/api # to be changed

# Targets
TARGET		= client 

# Library targets
QUEUE 		= $(UTIL_LIBS)/libqueue.so
PROTOCOL	= $(UTIL_LIBS)/libprotocol.so
API 		= $(API_LIBS)/libfilestorage_api.so

# Linking
LINK_API 	= -Wl,-rpath,$(API_LIBS)
LINK_API	+= -L $(API_LIBS)
LINK_API	+= -lfilestorage_api

LINK_UTILS 	= -Wl,-rpath,$(UTIL_LIBS)
LINK_UTILS	+= -L $(UTIL_LIBS)
LINK_UTILS	+= -lqueue
LINK_UTILS  += -lprotocol

 
LINK_ALL	= $(LINK_API)
LINK_ALL	+= $(LINK_UTILS)

# Flags
ifndef INCLUDES
INCLUDES	= -I$(INCDIR)
endif

ifndef DEBUG
DEBUG		= -DDEBUG=true -g
endif


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

# Parameter list for debugging and memchecking
PARAMS	= -R 10 -W file1,file2,file3,file4 -r file1,file2,file3,file4 \
		   -l lock1,lock2 -u ulock2,ulock3 -c clean1,clean2 -t 10 -p \
		   -f ./tmp/filestorage.sk -D trash_here/ -d read_here/

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
.PHONY: clean memcheck debug cleanall
.DEFAULT_GOAL := all

all: setup $(BIN)
setup: dir
	
remake: clean all

dir:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

debug: $(BIN)
debug: CFLAGS += $(DEBUG)
debug: 
	@echo "Running client with options: $(PARAMS)"
	./$(BIN) $(PARAMS)

memcheck: $(BIN)
	@echo "Running valgrind on client with options: $(PARAMS)"
	@valgrind --leak-check=full ./$(BIN) $(PARAMS)

clean:
	$(RM) $(OBJ) $(DEP)

cleanall:
	$(RM) $(OBJ) $(DEP) $(BIN)
