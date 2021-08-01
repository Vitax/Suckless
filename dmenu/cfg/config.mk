VERSION = 4.8

PREFIX = ${HOME}/.local
MANPREFIX = ${PREFIX}/share/man

# OpenBSD
# X11INC = /usr/X11R6/include
# X11LIB = /usr/X11R6/lib

# FreeBSD
X11INC = /usr/local/include
X11LIB = /usr/local/lib

XINERAMALIBS  = -lXinerama
XINERAMAFLAGS = -DXINERAMA

FREETYPELIBS = -lfontconfig -lXft

# ---------- LINUX -------------
FREETYPELIBS = -lfontconfig -lXft
#FREETYPEINC = /usr/include/freetype2

# --------- BSD ---------------
FREETYPEINC = $(X11INC)/freetype2

INCS = -I${X11INC} \
	   -I${FREETYPEINC} \
	   -I${XDG_CACHE_HOME}/themes/

LIBS = -L${X11LIB} \
	   -lX11 ${XINERAMALIBS} ${FREETYPELIBS} -lm

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
CFLAGS   = -O2 -pipe -fstack-protector-strong -std=c99 -pedantic -Wall ${INCS} ${CPPFLAGS}
LDFLAGS  = -s ${LIBS}

CC = cc
