// imports
#include <dwm.h>

#define STATUSBAR "status_info"

/*
 * appearance
 */
/* horizontal padding between the underline and tag */
static const unsigned int ulinepad = 6;
/* thickness / height of the underline */
static const unsigned int ulinestroke = 4;
/* how far above the bottom of the bar the line should appear */
static const unsigned int ulinevoffset = 2;
/* 1 to show underline on all tags, 0 for just the active ones */
static const int ulineall = 0;

/* gaps between windows */
static const unsigned int gappx = 16;
/* border pixel of windows */
static const unsigned int borderpx = 3;

/* enable bar padding */
static const int barpadding = 0;
/* vertical padding of bar */
static const int vertbarpad = barpadding ? gappx / 2 : 0;
/* horizontal padding of bar */
static const int horizbarpad = barpadding ? gappx : 0;

/* vertical padding of tab */
static const int verttabpad = 4;
/* horizontal padding of tab */
static const int horiztabpad = 4;

/* vertical padding on text */
static const int verttxtpad = 4;
/* horizontal padding on text */
static const int horiztxtpad = 4;

/* user defined bar height */
static const int barheight = 28;

/* 0 means no bar */
static const int showbar = 1;
/* 0 means bottom bar */
static const int topbar = 1;

/* 1 means respect decoration hints */
static const int decorhints = 1;

/* Possible tab bar modes */
enum showtab_modes {
  showtab_never,
  showtab_auto,
  showtab_nmodes,
  showtab_always
};

/* User defined tab bar height */
static const int tabheight = 24;
/* Default tab bar show mode */
static const int showtab = showtab_auto;
/* 0 means bottom tab bar */
static const int toptab = 1;

static const char *fonts[] = {
    "Fantasque Sans Mono:pixelsize=18:antialias=true",
    "JetBrainsMono Nerd Font:pixelsize=18:antialias=true",
};

typedef struct {
  const char *name;
  const void *cmd;
} Sp;

/* const char *spcmd1[] = {"st",   "-n",  "spterm", "-g", "152x44", "-e", */
/*                         "tmux", "new", "-A",     "-s", "spterm", NULL}; */
/* const char *spcmd2[] = {"st",     "-n", "spfm",   "-g", */
/*                         "110x30", "-e", "ranger", NULL}; */
const char *spcmd1[] = {"kitty", "-o", "remember_window_size=no",
			"-o", "initial_window_width=1420", "-o",
                        "initial_window_height=820",
                        "--name", "spterm",
                        "-e", "tmux", "new", "-A", "-s", "spterm", NULL};
const char *spcmd2[] = {"kitty", "-o", "remember_window_size=no",
                        "-o", "initial_window_width=840",
                        "-o", "initial_window_height=600",
                        "--name", "spfm", "-e", "ranger", NULL};
const char *spcmd3[] = {"keepassxc", NULL};
const char *spcmd4[] = {"stalonetray", NULL};

static Sp scratchpads[] = {
    /* name, cmd  */
    {"spterm", spcmd1},
    {"spranger", spcmd2},
    {"keepassxc", spcmd3},
    {"stalonetray", spcmd4},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6", "0"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */

    /* class/instance, title, tags, mask, iscentered, isfloating, monitor */
    {"sxiv", NULL, NULL, 0, 1, 1, -1},
    {"Sxiv", NULL, NULL, 0, 1, 1, -1},
    {"pcmanfm", NULL, NULL, 0, 1, 1, -1},
    {"Pcmanfm", NULL, NULL, 0, 1, 1, -1},
    {NULL, "spterm", NULL, SPTAG(0), 1, 1, -1},
    {NULL, "spfm", NULL, SPTAG(1), 1, 1, -1},
    {"KeePassXC", NULL, NULL, SPTAG(2), 1, 1, -1},
    {"stalonetray", NULL, NULL, SPTAG(3), 1, 1, -1},
};

/* layout(s) */
static const int nmaster = 1;
/* 1 means respect size hints in tiled resizals */
static const int resizehints = 1;
/* 1 will force focus on the fullscreen window */
static const int lockfullscreen = 1;

/* factor of master area size [0.05..0.95] */
static const float mfact = 0.50;

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"[T]", tile},
    {"><>", NULL},
    {"[M] ", monocle},
    {"TTT ", bstack},
};

/* key definitions */
#define AltMask Mod1Mask
#define SuperMask Mod4Mask

#define TAGKEYS(KEY, TAG)                                                      \
  {SuperMask, KEY, view, {.ui = 1 << TAG}},                                    \
      {SuperMask | ShiftMask, KEY, tag, {.ui = 1 << TAG}},

static Key keys[] = {
    /* modifier, key, function, argument */
    {SuperMask | ControlMask, XK_b, togglebar, {0}},

    {SuperMask, XK_j, focusstack, {.i = +1}},
    {SuperMask, XK_k, focusstack, {.i = -1}},

    {SuperMask, XK_h, rotatestack, {.i = -1}},
    {SuperMask, XK_l, rotatestack, {.i = +1}},

    {SuperMask, XK_i, incnmaster, {.i = +1}},
    {SuperMask, XK_d, incnmaster, {.i = -1}},

    {SuperMask, XK_w, tabmode, {-1}},

    {SuperMask | ShiftMask, XK_h, setmfact, {.f = -0.0125}},
    {SuperMask | ShiftMask, XK_l, setmfact, {.f = +0.0125}},

    {AltMask, XK_Tab, view, {0}},

    {SuperMask | ShiftMask, XK_c, killclient, {0}},
    {SuperMask, XK_F4, killclient, {0}},

    {SuperMask | ShiftMask, XK_t, setlayout, {.v = &layouts[0]}},
    {SuperMask | ShiftMask, XK_f, setlayout, {.v = &layouts[1]}},
    {SuperMask | ShiftMask, XK_m, setlayout, {.v = &layouts[2]}},
    {SuperMask | ShiftMask, XK_r, setlayout, {.v = &layouts[3]}},

    {SuperMask | ShiftMask, XK_y, togglescratch, {.ui = 0}},
    {SuperMask | ShiftMask, XK_u, togglescratch, {.ui = 1}},
    {SuperMask | ShiftMask, XK_x, togglescratch, {.ui = 2}},
    {SuperMask | ShiftMask, XK_i, togglescratch, {.ui = 3}},

    {SuperMask | ControlMask, XK_m, togglefullscr, {0}},
    {SuperMask | ControlMask, XK_s, togglesticky, {0}},
    {SuperMask | ControlMask, XK_space, togglefloating, {0}},

    {SuperMask, XK_comma, focusmon, {.i = -1}},
    {SuperMask, XK_period, focusmon, {.i = +1}},

    {SuperMask | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {SuperMask | ShiftMask, XK_period, tagmon, {.i = +1}},

    {SuperMask, XK_g, setgaps, {.i = +4}},
    {SuperMask | ShiftMask, XK_g, setgaps, {.i = -4}},

    {SuperMask | AltMask | ShiftMask, XK_q, quit, {0}},

    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5)
    /* TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7) TAGKEYS(XK_8, 9) */
    TAGKEYS(XK_0, 6)};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click, event mask, button, function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkClientWin, SuperMask, Button1, movemouse, {0}},
    {ClkClientWin, SuperMask, Button2, togglefloating, {0}},
    {ClkClientWin, SuperMask, Button3, resizemouse, {0}},
    {ClkStatusText, 0, Button1, sigstatusbar, {.i = 1}},
    {ClkStatusText, 0, Button2, sigstatusbar, {.i = 2}},
    {ClkStatusText, 0, Button3, sigstatusbar, {.i = 3}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTabBar, 0, Button1, focuswin, {0}},
    {ClkTagBar, SuperMask, Button1, tag, {0}},
    {ClkTagBar, SuperMask, Button3, toggletag, {0}},
};
