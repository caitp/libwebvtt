objdir="$arch-win32-msvc$msvcver"

all: configure

install: configure

installcheck: configure

dist: configure

distcheck: configure



.IGNORE: all install installcheck dist distcheck