#include <dmenu.h>

/* See LICENSE file for copyright and license details. */
/* Default settings; can be overriden by command line. */

/* -b  option; if 0, dmenu appears at bottom     */
static int topbar = 1;

/* set border width of menu */
static const unsigned int border_width = 3;

/* -fn option overrides fonts[0]; default X11 font or font set */
static const char *fonts[] =
{
    "JetBrains Mono:pixelsize=18:antialias=true",
    "JetBrainsMono Nerd Font:pixelsize=18:antialias=true"
};

/* -p  option; prompt to the left of input field */
static const char *prompt = "";

/* -l option; if nonzero, dmenu uses vertical list with given number of lines */
static unsigned int lines = 12;
static unsigned int lineheight = 24;

/*
 * Characters not considered part of a word while deleting words
 * for example: " /?\"&[]"
 */
static const char worddelimiters[] = " /?\"&[]";
