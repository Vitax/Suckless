VERSION = 1.4

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# OpenBSD
X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# FreeBSD
# X11INC = /usr/local/include
# X11LIB = /usr/local/lib

XINERAMALIBS  = -lXinerama
XINERAMAFLAGS = -DXINERAMA
FREETYPELIBS = -lXft

# OpenBSD
# FREETYPEINC = /usr/X11R6/include/freetype2

# FreeBSD
# FREETYPEINC = /usr/local/include/freetype2

# Linux
FREETYPEINC = /usr/include/freetype2

INCS = -I. -I/usr/include -I${X11INC} -I${FREETYPEINC} -I/home/vitax/.cache/themes/

CFLAGS = -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS = -s ${LIBS}

# OpenBSD
# LIBS = -L/usr/lib -lc -L${X11LIB} -lX11 -lXext -lXrandr ${XINERAMALIBS} ${FREETYPELIBS}
# CPPFLAGS = -DVERSION=\"${VERSION}\" -D_BSD_SOURCE ${XINERAMAFLAGS} -DHAVE_BSD_AUTH
#
# Linux
LIBS = -L/usr/lib -lc -lcrypt -L${X11LIB} -lX11 -lXext -lXrandr ${XINERAMALIBS} ${FREETYPELIBS}
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_DEFAULT_SOURCE -DHAVE_SHADOW_H ${XINERAMAFLAGS}

# NetBSD
# CPPFLAGS = -DVERSION=\"${VERSION}\" -D_BSD_SOURCE -D_NETBSD_SOURCE ${XINERAMAFLAGS}

# OpenBSD
# COMPATSRC =
# Linux / FreeBSD / NetBSD
COMPATSRC = explicit_bzero.c

CC = cc
