SHELL = /bin/bash
PYTHON := $(shell which python)
CFLAGS += -I./include -ggdb -Wall
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
TESTDIR := ./test/
SPECDIR := $(TESTDIR)spec/
VTTDIR := $(TESTDIR)vtt/
TESTS := $(shell find $(SPEC_DIR) -name '*.test' -print)
VTTS := $(patsubst $(SPECDIR)%.test,$(VTTDIR)%.vtt,$(TESTS))
LIB := .a
EXE := .exe
DLL := .dll

STIPVTT := $(SPECDIR)strip-vtt.py
TESTHARNESS := $(SPECDIR)run-tests-c.py
PARSEVTT := $(BINDIR)parsevtt$(EXE)
LIBWEBVTT := $(LIBDIR)libwebvtt$(LIB)

all: libwebvtt parsevtt

clean:
	-$(RM) $(BINDIR)parsevtt $(LIBDIR)libwebvtt $(OBJECTS) $(OBJECTS1)

libwebvtt: $(LIBWEBVTT)
	
parsevtt: $(PARSEVTT)

check-js: objdir $(VTT_SRC)
	$(PYTHON) ./test/spec/run-tests-js.py $(OBJ_DIR_SPEC)

$(VTTDIR):
	-mkdir $@ > /dev/null
	
$(VTTDIR)%.vtt : $(SPECDIR)%.test
	@$(PYTHON) $(STIPVTT) $< $@

test: $(PARSEVTT) $(VTTDIR) $(VTTS)
	$(PYTHON) $(TESTHARNESS) $(VTTDIR) $(PARSEVTT)
	
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