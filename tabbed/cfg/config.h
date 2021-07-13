/* See LICENSE file for copyright and license details. */

/* appearance */
/* static const char font[]        = "Iosevka:pixelsize=14:antialias=true"; */
static char font[] = "DejaVu Sans Mono:pixelsize=16:antialias=true:autohint=full";

#include <tabbed.h>

static const char before[]      = "<";
static const char after[]       = ">";
static const char titletrim[]   = "...";
static const int  tabwidth      = 200;
static const Bool foreground    = True;
static       Bool urgentswitch  = False;

/*
 * Where to place a new tab when it is opened. When npisrelative is True,
 * then the current position is changed + newposition. If npisrelative
 * is False, then newposition is an absolute position.
 */
static int  newposition   = 0;
static Bool npisrelative  = False;

#define SETPROP(p) { \
        .v = (char *[]){ "/bin/sh", "-c", \
                "prop=\"`xwininfo -children -id $1 | grep '^     0x' |" \
                "sed -e's@^ *\\(0x[0-9a-f]*\\) \"\\([^\"]*\\)\".*@\\1 \\2@' |" \
                "xargs -0 printf %b | dmenu -l 10 -w $1`\" &&" \
                "xprop -id $1 -f $0 8s -set $0 \"$prop\"", \
                p, winid, NULL \
        } \
}

#define shift ShiftMask
#define ctrl ControlMask

static Key keys[] = {
	/* modifier             key        function     argument */
	{ ctrl|shift, XK_Return, focusonce,   { 0 } },
	{ ctrl|shift, XK_Return, spawn,       { 0 } },

	{ ctrl|shift, XK_l, rotate,      { .i = +1 } },
	{ ctrl|shift, XK_h, rotate,      { .i = -1 } },
	{ ctrl|shift, XK_j, movetab,     { .i = -1 } },
	{ ctrl|shift, XK_k, movetab,     { .i = +1 } },
	{ ctrl, XK_Tab,    rotate,      { .i = 0 } },

	{ ctrl|shift, XK_grave, spawn, SETPROP("_TABBED_SELECT_TAB") },
	{ ctrl, XK_1, move, { .i = 0 } },
	{ ctrl, XK_2, move, { .i = 1 } },
	{ ctrl, XK_3, move, { .i = 2 } },
	{ ctrl, XK_4, move, { .i = 3 } },
	{ ctrl, XK_5, move, { .i = 4 } },
	{ ctrl, XK_6, move, { .i = 5 } },
	{ ctrl, XK_7, move, { .i = 6 } },
	{ ctrl, XK_8, move, { .i = 7 } },
	{ ctrl, XK_9, move, { .i = 8 } },
	{ ctrl, XK_0, move, { .i = 9 } },

	{ ctrl, XK_q,      killclient,  { 0 } },

	{ ctrl,               XK_u,      focusurgent, { 0 } },
	{ ctrl|shift,     XK_u,      toggle,      { .v = (void*) &urgentswitch } },

	{ 0,                    XK_F11,    fullscreen,  { 0 } },
};
