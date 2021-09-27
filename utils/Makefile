# General
CC			= gcc
LD			= gcc
RM			= rm -rf

# Directories
SRCDIR		= src
INCDIR		= include
OBJDIR		= obj
LIBDIR 		= libs

# Library targets
QUEUE 		= $(LIBDIR)/libqueue.so

# Flags
CFLAGS		:= -Wall -std=c99 -pedantic -g
INCLUDES	= -I$(INCDIR)

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
$(LIBDIR)/%.so: $(OBJ)
	$(CC) -shared -o $@ $<

.PHONY: queue clean cleanall
.DEFAULT_GOAL := all

all: 
	@make queue

queue: $(QUEUE)

clean:
	$(RM) $(OBJ) $(DEP) 

cleanall:
	$(RM) $(OBJ) $(DEP) $(QUEUE)