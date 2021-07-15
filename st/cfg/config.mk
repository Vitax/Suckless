# st version
VERSION = 0.8.4

# paths
PREFIX = ${HOME}/.local
MANPREFIX = $(PREFIX)/share/man

PKG_CONFIG = pkg-config

# OpenBSD
# X11INC = /usr/X11R6/include
# X11LIB = /usr/X11R6/lib

# FreeBSD
X11INC = /usr/local/include
X11LIB = /usr/local/lib

# includes and libs
INCS = -I$(X11INC) \
	   `$(PKG_CONFIG) --cflags fontconfig` \
	   `$(PKG_CONFIG) --cflags freetype2` \
	   -I${XDG_CACHE_HOME}/themes

# LIBS = -L$(X11LIB) -lm -lrt -lX11 -lutil -lXft \
#        `$(PKG_CONFIG) --libs fontconfig` \
#        `$(PKG_CONFIG) --libs freetype2`

# flags
CPPFLAGS = -DVERSION=\"$(VERSION)\" -D_XOPEN_SOURCE=600
STCFLAGS = $(INCS) $(STCPPFLAGS) $(CPPFLAGS) $(CFLAGS)
STLDFLAGS = $(LIBS) $(LDFLAGS)

# OpenBSD:
# CPPFLAGS = -DVERSION=\"$(VERSION)\" -D_XOPEN_SOURCE=600 -D_BSD_SOURCE
LIBS = -L$(X11LIB) -lm -lX11 -lutil -lXft \
		`$(PKG_CONFIG) --libs fontconfig` \
		`$(PKG_CONFIG) --libs freetype2`

# compiler and linker
CC = cc
