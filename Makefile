SHELL = /bin/bash
CFLAGS += -I./include -ggdb
SRCDIR := ./src/
SOURCES := $(shell find $(SRCDIR)libwebvtt/*.c)
OBJDIR := ./obj/
OBJECTS := $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SOURCES))
OBJDIRS := $(sort $(dir $(OBJECTS)))
LIBDIR := ./lib/
BINDIR := ./bin/
SOURCES1 := $(shell find $(SRCDIR)parsevtt/*.c)
OBJECTS1 := $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SOURCES1))
OBJDIRS1 := $(sort $(dir $(OBJECTS1)))
TESTS := $(wildcard tests/*.vtt)

LIB := .a
EXE := .exe
DLL := .dll

PARSEVTT := $(BINDIR)parsevtt$(EXE)
LIBWEBVTT := $(LIBDIR)libwebvtt$(LIB)

all: libwebvtt parsevtt

clean:
	-$(RM) $(BINDIR)parsevtt $(LIBDIR)libwebvtt $(OBJECTS) $(OBJECTS1)

libwebvtt: $(LIBWEBVTT)
	
parsevtt: $(PARSEVTT)

test: $(PARSEVTT)
	$(foreach T,$(TESTS),$(shell $(PARSEVTT) -f $T))
	
$(LIBWEBVTT): $(OBJECTS) | $(LIBDIR)
	$(AR) rcs $(LIBWEBVTT) $^
	
$(PARSEVTT): $(LIBWEBVTT) $(OBJECTS1) | $(BINDIR)
	$(CC) $(OBJECTS1) -L$(LIBDIR) -lwebvtt -o $@ $(CFLAGS)
	
$(LIBDIR):
	-mkdir $@ 2>/dev/null

$(BINDIR):
	-mkdir $@ 2>/dev/null

$(OBJDIR):
	-mkdir $@ 2>/dev/null
	
$(OBJDIRS): | $(OBJDIR)
	-mkdir $@ 2>/dev/null

$(OBJDIRS1): | $(OBJDIR)
	-mkdir $@ 2>/dev/null
	
$(OBJDIR)%.o: $(SRCDIR)%.c | $(OBJDIRS) $(OBJDIRS1)
	$(CC) -c $< -o $@ $(CFLAGS)
	
	
.PHONY: all libwebvtt parsevtt clean test