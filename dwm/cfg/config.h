/* See LICENSE file for copyright and license details. */

#include <dwm.h>

/* appearance */
/* gaps between windows */
static const unsigned int gappx = 0;
/* border pixel of windows */
static const unsigned int borderpx = 4;

static const int vertpadbar = 2;
static const int horizpadbar = 4;

/* user defined bar height */
static const int barheight = 26;

/* 0 means no bar */
static const int showbar = 1;
/* 0 means bottom bar */
static const int topbar = 0;

static const char *fonts[] = {
    "FantasqueSansMono Nerd Font:pixelsize=18:antialias=true:autohint=full",
    "Feather:pixelsize=18:antialias=true:autohint=full",
};

typedef struct {
  const char *name;
  const void *cmd;
} Sp;

const char *spcmd1[] = {"st",   "-n",  "spterm", "-g", "144x41", "-e",
                        "tmux", "new", "-A",     "-s", "spterm", NULL};
const char *spcmd2[] = {"st",     "-n", "spfm",   "-g",
                        "120x34", "-e", "ranger", NULL};
const char *spcmd3[] = {"keepass.sh", NULL};

static Sp scratchpads[] = {
    /* name, cmd  */
    {"spterm", spcmd1},
    {"spranger", spcmd2},
    {"KeePass", spcmd3},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5", "6"};

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */

    /* class/instance, title, tags, mask, iscentered, isfloating, monitor */
    {"FileSearch", NULL, NULL, 0, 1, 1, -1},
    {"feh", NULL, NULL, 0, 1, 1, -1},
    {"pcmanfm", NULL, NULL, 0, 1, 1, -1},
    {"Pcmanfm", NULL, NULL, 0, 1, 1, -1},
    {"mpv", NULL, NULL, 0, 1, 1, -1},
    {"arandr", NULL, NULL, 0, 1, 1, -1},
    {"Arandr", NULL, NULL, 0, 1, 1, -1},
    {"sxiv", NULL, NULL, 0, 1, 1, -1},
    {"Sxiv", NULL, NULL, 0, 1, 1, -1},
    {"htop", NULL, NULL, 0, 1, 1, -1},
    {NULL, "spterm", NULL, SPTAG(0), 1, 1, -1},
    {NULL, "spfm", NULL, SPTAG(1), 1, 1, -1},
    {"KeePass", NULL, NULL, SPTAG(2), 1, 1, -1},
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
    {"[M]", monocle},
    {"TTT", bstack},
};

/* key definitions */
#define AltMask Mod1Mask
#define SuperMask Mod4Mask

#define TAGKEYS(KEY, TAG) \
    {AltMask, KEY, view, {.ui = 1 << TAG}}, \
    {AltMask | ShiftMask, KEY, tag, {.ui = 1 << TAG}},

static Key keys[] = {
    /* modifier, key, function, argument */
    {AltMask | ControlMask, XK_b, togglebar, {0}},

    {AltMask, XK_j, focusstackvis, {.i = +1}},
    {AltMask, XK_k, focusstackvis, {.i = -1}},

    {AltMask | ShiftMask, XK_j, focusstackhid, {.i = +1}},
    {AltMask | ShiftMask, XK_k, focusstackhid, {.i = -1}},

    {AltMask, XK_s, show, {0}},
    {AltMask | ShiftMask, XK_n, hide, {0}},

    {AltMask, XK_h, rotatestack, {.i = -1}},
    {AltMask, XK_l, rotatestack, {.i = +1}},

    {AltMask, XK_i, incnmaster, {.i = +1}},
    {AltMask, XK_d, incnmaster, {.i = -1}},

    {AltMask | ShiftMask, XK_h, setmfact, {.f = -0.0125}},
    {AltMask | ShiftMask, XK_l, setmfact, {.f = +0.0125}},

    {AltMask, XK_Tab, view, {0}},

    {AltMask | ShiftMask, XK_c, killclient, {0}},
    {AltMask, XK_F4, killclient, {0}},

    {AltMask, XK_t, setlayout, {.v = &layouts[0]}},
    {AltMask, XK_f, setlayout, {.v = &layouts[1]}},
    {AltMask, XK_m, setlayout, {.v = &layouts[2]}},
    {AltMask, XK_u, setlayout, {.v = &layouts[3]}},

    {AltMask | ShiftMask, XK_y, togglescratch, {.ui = 0}},
    {AltMask | ShiftMask, XK_u, togglescratch, {.ui = 1}},
    {AltMask | ShiftMask, XK_x, togglescratch, {.ui = 2}},

    {AltMask | ShiftMask, XK_m, togglefullscr, {0}},
    {AltMask | ShiftMask, XK_s, togglesticky, {0}},
    {AltMask | ShiftMask, XK_space, togglefloating, {0}},

    {AltMask, XK_comma, focusmon, {.i = -1}},
    {AltMask, XK_period, focusmon, {.i = +1}},

    {AltMask | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {AltMask | ShiftMask, XK_period, tagmon, {.i = +1}},

    {AltMask | ShiftMask, XK_g, setgaps, {.i = +4}},
    {AltMask | ControlMask, XK_g, setgaps, {.i = -4}},

    {SuperMask | AltMask | ShiftMask, XK_q, quit, {0}},

    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5)
    /* TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7) TAGKEYS(XK_8, 9) */
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click, event mask, button, function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkClientWin, AltMask, Button1, movemouse, {0}},
    {ClkClientWin, AltMask, Button2, togglefloating, {0}},
    {ClkClientWin, AltMask, Button3, resizemouse, {0}},
    {ClkWinTitle, 0, Button1, togglewin, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, AltMask, Button1, tag, {0}},
    {ClkTagBar, AltMask, Button3, toggletag, {0}},
};
