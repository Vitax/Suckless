# sent version
VERSION = 1

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# OpenBSD
# X11INC = /usr/X11R6/include
# X11LIB = /usr/X11R6/lib

# FreeBSD
X11INC = /usr/local/include
X11LIB = /usr/local/lib

# Linux includes and libs
#LIBS = -L/usr/lib -lc -lm -L${X11LIB} -lXft -lfontconfig -lX11
#INCS = -I. -I/usr/include -I/usr/include/freetype2 -I${X11INC}

# BSD (uncomment)
INCS = -I. -I${X11INC} -I${X11INC}/freetype2

# FreeBSD (uncomment)
LIBS = -L/usr/local/lib -lc -lm -L${X11LIB} -lXft -lfontconfig -lX11

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_XOPEN_SOURCE=600
CFLAGS += -g -std=c99 -pedantic -Wall ${INCS} ${CPPFLAGS}
LDFLAGS += -g ${LIBS}
#CFLAGS += -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
#LDFLAGS += ${LIBS}

# compiler and linker
CC ?= cc
