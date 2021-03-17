# tabbed version
VERSION = 0.6

# Customize below to fit your system

# paths
PREFIX = ${HOME}/.local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# freetype
FREETYPELIBS = -lfontconfig -lXft

# Linux
# FREETYPEINC = /usr/include/freetype2

# OpenBSD (uncomment)
# FREETYPEINC = ${X11INC}/freetype2

# FreeBSD
FREETYPEINC = /usr/local/include/freetype2

# includes and libs
INCS = -I. -I/usr/include \
	   -I$(X11INC)  \
	   -I${FREETYPEINC} \
	   -I${XDG_CACHE_HOME}/themes/

LIBS = -L/usr/lib -lc -L${X11LIB} \
	   -lX11 ${FREETYPELIBS}

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" \
		   -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS = -s ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = cc
