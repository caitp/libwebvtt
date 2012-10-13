SHELL = /bin/bash
CFLAGS += -I./include -ggdb
SRCDIR := ./src/
SOURCES := $(SRCDIR)libwebvtt/webvtt_parser.c $(SRCDIR)libwebvtt/readers/webvtt_filereader.c
OBJDIR := ./obj/
OBJECTS := $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SOURCES))
OBJDIRS := $(sort $(dir $(OBJECTS)))
LIBDIR := ./lib/
BINDIR := ./bin/
SOURCES1 := $(SRCDIR)parsevtt/parsevtt_main.c
OBJECTS1 := $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SOURCES1))
OBJDIRS1 := $(sort $(dir $(OBJECTS1)))
TESTS := $(wildcard tests/*.vtt)

all: libwebvtt parsevtt

clean:
	-$(RM) $(BINDIR)parsevtt $(LIBDIR)libwebvtt $(OBJECTS) $(OBJECTS1)

libwebvtt: $(OBJECTS) | $(LIBDIR)
	$(AR) rcs $(LIBDIR)libwebvtt.a $^
	
parsevtt: $(OBJECTS1) libwebvtt | $(BINDIR)
	$(CC) $(OBJECTS1) -L$(LIBDIR) -lwebvtt -o $(BINDIR)$@ $(CFLAGS)

test: parsevtt
	$(foreach T,$(TESTS),$(shell $(BINDIR)parsevtt -f $T))
	
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