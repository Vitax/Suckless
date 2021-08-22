/* See LICENSE file for copyright and license details. */

#include <st.h>

/*
 * appearance
 */

static char font[] = "Iosevka Nerd Font:pixelsize=16:antialias=true";

// plumber patch
static char *plumb_cmd = "opn";

// look into using scroll later!
char *scroll = NULL;

static int borderpx = 32;

/*
 * 1: render most of the lines/blocks characters without using the font for
 *    perfect alignment between cells (U2500 - U259F except dashes/diagonals).
 *    Bold affects lines thickness if boxdraw_bold is not 0. Italic is ignored.
 * 0: disable (render all U25XX glyphs normally from the font).
 */
const int boxdraw = 0;
const int boxdraw_bold = 0;

/* braille (U28XX):  1: render as adjacent "pixels",  0: use font */
const int boxdraw_braille = 1;


// security concerns regarding window operations
// see: https://git.suckless.org/st/commit/a2a704492b9f4d2408d180f7aeeacf4c789a1d67.html
int allowwindowops = 0;
/*
 * What program is execed by st depends of these precedence rules:
 * 1: program passed with -e
 * 2: utmp option
 * 3: SHELL environment variable
 * 4: value of shell in /etc/passwd
 * 5: value of shell in config.h
 */
static char *shell = "/bin/sh";
char *utmp = NULL;
char *stty_args = "stty raw pass8 nl -echo -iexten -cstopb 38400";

/* identification sequence returned in DA and DECID */
char *vtiden = "\033[?6c";

/* Kerning / character bounding-box multipliers */
static float cwscale = 1.0;
static float chscale = 1.0;

/*
 * word delimiter string
 *
 * More advanced example: L" `'\"()[]{}"
 */
wchar_t *worddelimiters = L" `'\"()[]{}";

/* selection timeouts (in milliseconds) */
static unsigned int doubleclicktimeout = 300;
static unsigned int tripleclicktimeout = 600;

/* alt screens */
int allowaltscreen = 1;

/* frames per second st should at maximum draw to the screen */
static unsigned int xfps = 120;
static unsigned int actionfps = 30;

/*
 * blinking timeout (set to 0 to disable blinking) for the terminal blinking
 * attribute.
 */
static unsigned int blinktimeout = 800;

/*
 * thickness of underline and bar cursors
 */
static unsigned int cursorthickness = 2;

/*
 * bell volume. It must be a value between -100 and 100. Use 0 for disabling
 * it
 */
static int bellvolume = 0;

/* default TERM value */
char *termname = "st-256color";

/*
 * spaces per tab
 *
 * When you are changing this value, don't forget to adapt the »it« value in
 * the st.info and appropriately install the st.info in the environment where
 * you use this st version.
 *
 *	it#$tabspaces,
 *
 * Secondly make sure your kernel is not expanding tabs. When running `stty
 * -a` »tab0« should appear. You can tell the terminal to not expand tabs by
 *  running following command:
 *
 *	stty tabs
 */
unsigned int tabspaces = 4;
static double minlatency = 8;
static double maxlatency = 33;

/*
 * Default shape of cursor
 * 2: Block ("█")
 * 4: Underline ("_")
 * 6: Bar ("|")
 * 7: Snowman ("☃")
 */
static unsigned int cursorshape = 2;
static unsigned int cursorstyle = 2;
static Rune stcursor = 0x2603; /* snowman (U+2603) */

/*
 * Default columns and rows numbers
 */
static unsigned int cols = 76;
static unsigned int rows = 20;

/*
 * Default colour and shape of the mouse cursor
 */
static unsigned int mouseshape = XC_xterm;

/*
 * Color used to display font attributes when fontconfig selected a font which
 * doesn't match the ones requested.
 */
static unsigned int defaultattr = 11;

#define altMask Mod1Mask
#define superMask Mod4Mask
#define shiftMask ShiftMask
#define ctrlMask ControlMask
#define comb (ctrlMask|shiftMask)

/*
 * Force mouse select/shortcuts while mask is active (when MODE_MOUSE is set).
 * Note that if you want to use shiftMask with selmasks, set this to an other
 * modifier, set to 0 to not use it.
 */
static uint forcemousemod = shiftMask;

/*
 * Internal mouse shortcuts.
 * Beware that overloading Button1 will disable the selection.
 */
static MouseShortcut mshortcuts[] = {
    /* mask                 button   function        argument       release */
    {shiftMask, Button4, kscrollup, {.i = 1}},
    {shiftMask, Button5, kscrolldown, {.i = 1}},
    {XK_ANY_MOD, Button2, selpaste, {.i = 0}, 1},
    /*
     * {XK_ANY_MOD, Button4, ttysend, {.s = "\031"}},
     *{XK_ANY_MOD, Button5, ttysend, {.s = "\005"}},
     */
};

/* Internal keyboard shortcuts. */
static Shortcut shortcuts[] = {
    /* mask                 keysym          function        argument */
    {XK_ANY_MOD, XK_Break, sendbreak, {.i = 0}},
    {ctrlMask, XK_Print, toggleprinter, {.i = 0}},
    {shiftMask, XK_Print, printscreen, {.i = 0}},
    {XK_ANY_MOD, XK_Print, printsel, {.i = 0}},

    {comb, XK_Prior, zoom, {.f = +1}},
    {comb, XK_Next, zoom, {.f = -1}},
    {comb, XK_Home, zoomreset, {.f = 0}},

    {comb, XK_C, clipcopy, {.i = 0}},
    {comb, XK_V, clippaste, {.i = 0}},
    {comb, XK_Y, selpaste, {.i = 0}},

    {shiftMask, XK_Insert, selpaste, {.i = 0}},

    {comb, XK_Num_Lock, numlock, {.i = 0}},

    {shiftMask, XK_Page_Up, kscrollup, {.i = -1}},
    {shiftMask, XK_Page_Down, kscrolldown, {.i = -1}},

    {comb, XK_U, kscrollup, {.i = -1}},
    {comb, XK_D, kscrolldown, {.i = -1}},

    {comb, XK_K, kscrollup, {.i = +1}},
    {comb, XK_J, kscrolldown, {.i = +1}},
};

/*
 * Special keys (change & recompile st.info accordingly)
 *
 * Mask value:
 * * Use XK_ANY_MOD to match the key no matter modifiers state
 * * Use XK_NO_MOD to match the key alone (no modifiers)
 * appkey value:
 * * 0: no value1
 * * > 0: keypad application mode enabled
 * *   = 2: term.numlock = 1
 * * < 0: keypad application mode disabled
 * appcursor value:
 * * 0: no value
 * * > 0: cursor application mode enabled
 * * < 0: cursor application mode disabled
 *
 * Be careful with the order of the definitions because st searches in
 * this table sequentially, so any XK_ANY_MOD must be in the last
 * position for a key.
 */

/*
 * If you want keys other than the X11 function keys (0xFD00 - 0xFFFF)
 * to be mapped below, add them to this array.
 */
static KeySym mappedkeys[] = {-1};

/*
 * State bits to ignore when matching key or button events.  By default,
 * numlock (Mod2Mask) and keyboard layout (XK_SWITCH_MOD) are ignored.
 */
static uint ignoremod = Mod2Mask | XK_SWITCH_MOD;

/*
 * This is the huge key array which defines all compatibility to the Linux
 * world. Please decide about changes wisely.
 */
static Key key[] = {
    /* keysym           mask            string      appkey appcursor */
    {XK_KP_Home, shiftMask, "\033[2J", 0, -1},
    {XK_KP_Home, shiftMask, "\033[1;2H", 0, +1},
    {XK_KP_Home, XK_ANY_MOD, "\033[H", 0, -1},
    {XK_KP_Home, XK_ANY_MOD, "\033[1~", 0, +1},
    {XK_KP_Up, XK_ANY_MOD, "\033Ox", +1, 0},
    {XK_KP_Up, XK_ANY_MOD, "\033[A", 0, -1},
    {XK_KP_Up, XK_ANY_MOD, "\033OA", 0, +1},
    {XK_KP_Down, XK_ANY_MOD, "\033Or", +1, 0},
    {XK_KP_Down, XK_ANY_MOD, "\033[B", 0, -1},
    {XK_KP_Down, XK_ANY_MOD, "\033OB", 0, +1},
    {XK_KP_Left, XK_ANY_MOD, "\033Ot", +1, 0},
    {XK_KP_Left, XK_ANY_MOD, "\033[D", 0, -1},
    {XK_KP_Left, XK_ANY_MOD, "\033OD", 0, +1},
    {XK_KP_Right, XK_ANY_MOD, "\033Ov", +1, 0},
    {XK_KP_Right, XK_ANY_MOD, "\033[C", 0, -1},
    {XK_KP_Right, XK_ANY_MOD, "\033OC", 0, +1},
    {XK_KP_Prior, shiftMask, "\033[5;2~", 0, 0},
    {XK_KP_Prior, XK_ANY_MOD, "\033[5~", 0, 0},
    {XK_KP_Begin, XK_ANY_MOD, "\033[E", 0, 0},
    {XK_KP_End, ctrlMask, "\033[J", -1, 0},
    {XK_KP_End, ctrlMask, "\033[1;5F", +1, 0},
    {XK_KP_End, shiftMask, "\033[K", -1, 0},
    {XK_KP_End, shiftMask, "\033[1;2F", +1, 0},
    {XK_KP_End, XK_ANY_MOD, "\033[4~", 0, 0},
    {XK_KP_Next, shiftMask, "\033[6;2~", 0, 0},
    {XK_KP_Next, XK_ANY_MOD, "\033[6~", 0, 0},
    {XK_KP_Insert, shiftMask, "\033[2;2~", +1, 0},
    {XK_KP_Insert, shiftMask, "\033[4l", -1, 0},
    {XK_KP_Insert, ctrlMask, "\033[L", -1, 0},
    {XK_KP_Insert, ctrlMask, "\033[2;5~", +1, 0},
    {XK_KP_Insert, XK_ANY_MOD, "\033[4h", -1, 0},
    {XK_KP_Insert, XK_ANY_MOD, "\033[2~", +1, 0},
    {XK_KP_Delete, ctrlMask, "\033[M", -1, 0},
    {XK_KP_Delete, ctrlMask, "\033[3;5~", +1, 0},
    {XK_KP_Delete, shiftMask, "\033[2K", -1, 0},
    {XK_KP_Delete, shiftMask, "\033[3;2~", +1, 0},
    {XK_KP_Delete, XK_ANY_MOD, "\033[P", -1, 0},
    {XK_KP_Delete, XK_ANY_MOD, "\033[3~", +1, 0},
    {XK_KP_Multiply, XK_ANY_MOD, "\033Oj", +2, 0},
    {XK_KP_Add, XK_ANY_MOD, "\033Ok", +2, 0},
    {XK_KP_Enter, XK_ANY_MOD, "\033OM", +2, 0},
    {XK_KP_Enter, XK_ANY_MOD, "\r", -1, 0},
    {XK_KP_Subtract, XK_ANY_MOD, "\033Om", +2, 0},
    {XK_KP_Decimal, XK_ANY_MOD, "\033On", +2, 0},
    {XK_KP_Divide, XK_ANY_MOD, "\033Oo", +2, 0},
    {XK_KP_0, XK_ANY_MOD, "\033Op", +2, 0},
    {XK_KP_1, XK_ANY_MOD, "\033Oq", +2, 0},
    {XK_KP_2, XK_ANY_MOD, "\033Or", +2, 0},
    {XK_KP_3, XK_ANY_MOD, "\033Os", +2, 0},
    {XK_KP_4, XK_ANY_MOD, "\033Ot", +2, 0},
    {XK_KP_5, XK_ANY_MOD, "\033Ou", +2, 0},
    {XK_KP_6, XK_ANY_MOD, "\033Ov", +2, 0},
    {XK_KP_7, XK_ANY_MOD, "\033Ow", +2, 0},
    {XK_KP_8, XK_ANY_MOD, "\033Ox", +2, 0},
    {XK_KP_9, XK_ANY_MOD, "\033Oy", +2, 0},
    {XK_Up, shiftMask, "\033[1;2A", 0, 0},
    {XK_Up, altMask, "\033[1;3A", 0, 0},
    {XK_Up, shiftMask | altMask, "\033[1;4A", 0, 0},
    {XK_Up, ctrlMask, "\033[1;5A", 0, 0},
    {XK_Up, shiftMask | ctrlMask, "\033[1;6A", 0, 0},
    {XK_Up, ctrlMask | altMask, "\033[1;7A", 0, 0},
    {XK_Up, shiftMask | ctrlMask | altMask, "\033[1;8A", 0, 0},
    {XK_Up, XK_ANY_MOD, "\033[A", 0, -1},
    {XK_Up, XK_ANY_MOD, "\033OA", 0, +1},
    {XK_Down, shiftMask, "\033[1;2B", 0, 0},
    {XK_Down, altMask, "\033[1;3B", 0, 0},
    {XK_Down, shiftMask | altMask, "\033[1;4B", 0, 0},
    {XK_Down, ctrlMask, "\033[1;5B", 0, 0},
    {XK_Down, shiftMask | ctrlMask, "\033[1;6B", 0, 0},
    {XK_Down, ctrlMask | altMask, "\033[1;7B", 0, 0},
    {XK_Down, shiftMask | ctrlMask | altMask, "\033[1;8B", 0, 0},
    {XK_Down, XK_ANY_MOD, "\033[B", 0, -1},
    {XK_Down, XK_ANY_MOD, "\033OB", 0, +1},
    {XK_Left, shiftMask, "\033[1;2D", 0, 0},
    {XK_Left, altMask, "\033[1;3D", 0, 0},
    {XK_Left, shiftMask | altMask, "\033[1;4D", 0, 0},
    {XK_Left, ctrlMask, "\033[1;5D", 0, 0},
    {XK_Left, shiftMask | ctrlMask, "\033[1;6D", 0, 0},
    {XK_Left, ctrlMask | altMask, "\033[1;7D", 0, 0},
    {XK_Left, shiftMask | ctrlMask | altMask, "\033[1;8D", 0, 0},
    {XK_Left, XK_ANY_MOD, "\033[D", 0, -1},
    {XK_Left, XK_ANY_MOD, "\033OD", 0, +1},
    {XK_Right, shiftMask, "\033[1;2C", 0, 0},
    {XK_Right, altMask, "\033[1;3C", 0, 0},
    {XK_Right, shiftMask | altMask, "\033[1;4C", 0, 0},
    {XK_Right, ctrlMask, "\033[1;5C", 0, 0},
    {XK_Right, shiftMask | ctrlMask, "\033[1;6C", 0, 0},
    {XK_Right, ctrlMask | altMask, "\033[1;7C", 0, 0},
    {XK_Right, shiftMask | ctrlMask | altMask, "\033[1;8C", 0, 0},
    {XK_Right, XK_ANY_MOD, "\033[C", 0, -1},
    {XK_Right, XK_ANY_MOD, "\033OC", 0, +1},
    {XK_ISO_Left_Tab, shiftMask, "\033[Z", 0, 0},
    {XK_Return, altMask, "\033\r", 0, 0},
    {XK_Return, XK_ANY_MOD, "\r", 0, 0},
    {XK_Insert, shiftMask, "\033[4l", -1, 0},
    {XK_Insert, shiftMask, "\033[2;2~", +1, 0},
    {XK_Insert, ctrlMask, "\033[L", -1, 0},
    {XK_Insert, ctrlMask, "\033[2;5~", +1, 0},
    {XK_Insert, XK_ANY_MOD, "\033[4h", -1, 0},
    {XK_Insert, XK_ANY_MOD, "\033[2~", +1, 0},
    {XK_Delete, ctrlMask, "\033[M", -1, 0},
    {XK_Delete, ctrlMask, "\033[3;5~", +1, 0},
    {XK_Delete, shiftMask, "\033[2K", -1, 0},
    {XK_Delete, shiftMask, "\033[3;2~", +1, 0},
    {XK_Delete, XK_ANY_MOD, "\033[P", -1, 0},
    {XK_Delete, XK_ANY_MOD, "\033[3~", +1, 0},
    {XK_BackSpace, XK_NO_MOD, "\177", 0, 0},
    {XK_BackSpace, altMask, "\033\177", 0, 0},
    {XK_Home, shiftMask, "\033[2J", 0, -1},
    {XK_Home, shiftMask, "\033[1;2H", 0, +1},
    {XK_Home, XK_ANY_MOD, "\033[H", 0, -1},
    {XK_Home, XK_ANY_MOD, "\033[1~", 0, +1},
    {XK_End, ctrlMask, "\033[J", -1, 0},
    {XK_End, ctrlMask, "\033[1;5F", +1, 0},
    {XK_End, shiftMask, "\033[K", -1, 0},
    {XK_End, shiftMask, "\033[1;2F", +1, 0},
    {XK_End, XK_ANY_MOD, "\033[4~", 0, 0},
    {XK_Prior, ctrlMask, "\033[5;5~", 0, 0},
    {XK_Prior, shiftMask, "\033[5;2~", 0, 0},
    {XK_Prior, XK_ANY_MOD, "\033[5~", 0, 0},
    {XK_Next, ctrlMask, "\033[6;5~", 0, 0},
    {XK_Next, shiftMask, "\033[6;2~", 0, 0},
    {XK_Next, XK_ANY_MOD, "\033[6~", 0, 0},
    {XK_F1, XK_NO_MOD, "\033OP", 0, 0},
    {XK_F1, /* F13 */ shiftMask, "\033[1;2P", 0, 0},
    {XK_F1, /* F25 */ ctrlMask, "\033[1;5P", 0, 0},
    {XK_F1, /* F37 */ superMask, "\033[1;6P", 0, 0},
    {XK_F1, /* F49 */ altMask, "\033[1;3P", 0, 0},
    {XK_F1, /* F61 */ Mod3Mask, "\033[1;4P", 0, 0},
    {XK_F2, XK_NO_MOD, "\033OQ", 0, 0},
    {XK_F2, /* F14 */ shiftMask, "\033[1;2Q", 0, 0},
    {XK_F2, /* F26 */ ctrlMask, "\033[1;5Q", 0, 0},
    {XK_F2, /* F38 */ superMask, "\033[1;6Q", 0, 0},
    {XK_F2, /* F50 */ altMask, "\033[1;3Q", 0, 0},
    {XK_F2, /* F62 */ Mod3Mask, "\033[1;4Q", 0, 0},
    {XK_F3, XK_NO_MOD, "\033OR", 0, 0},
    {XK_F3, /* F15 */ shiftMask, "\033[1;2R", 0, 0},
    {XK_F3, /* F27 */ ctrlMask, "\033[1;5R", 0, 0},
    {XK_F3, /* F39 */ superMask, "\033[1;6R", 0, 0},
    {XK_F3, /* F51 */ altMask, "\033[1;3R", 0, 0},
    {XK_F3, /* F63 */ Mod3Mask, "\033[1;4R", 0, 0},
    {XK_F4, XK_NO_MOD, "\033OS", 0, 0},
    {XK_F4, /* F16 */ shiftMask, "\033[1;2S", 0, 0},
    {XK_F4, /* F28 */ ctrlMask, "\033[1;5S", 0, 0},
    {XK_F4, /* F40 */ superMask, "\033[1;6S", 0, 0},
    {XK_F4, /* F52 */ altMask, "\033[1;3S", 0, 0},
    {XK_F5, XK_NO_MOD, "\033[15~", 0, 0},
    {XK_F5, /* F17 */ shiftMask, "\033[15;2~", 0, 0},
    {XK_F5, /* F29 */ ctrlMask, "\033[15;5~", 0, 0},
    {XK_F5, /* F41 */ superMask, "\033[15;6~", 0, 0},
    {XK_F5, /* F53 */ altMask, "\033[15;3~", 0, 0},
    {XK_F6, XK_NO_MOD, "\033[17~", 0, 0},
    {XK_F6, /* F18 */ shiftMask, "\033[17;2~", 0, 0},
    {XK_F6, /* F30 */ ctrlMask, "\033[17;5~", 0, 0},
    {XK_F6, /* F42 */ superMask, "\033[17;6~", 0, 0},
    {XK_F6, /* F54 */ altMask, "\033[17;3~", 0, 0},
    {XK_F7, XK_NO_MOD, "\033[18~", 0, 0},
    {XK_F7, /* F19 */ shiftMask, "\033[18;2~", 0, 0},
    {XK_F7, /* F31 */ ctrlMask, "\033[18;5~", 0, 0},
    {XK_F7, /* F43 */ superMask, "\033[18;6~", 0, 0},
    {XK_F7, /* F55 */ altMask, "\033[18;3~", 0, 0},
    {XK_F8, XK_NO_MOD, "\033[19~", 0, 0},
    {XK_F8, /* F20 */ shiftMask, "\033[19;2~", 0, 0},
    {XK_F8, /* F32 */ ctrlMask, "\033[19;5~", 0, 0},
    {XK_F8, /* F44 */ superMask, "\033[19;6~", 0, 0},
    {XK_F8, /* F56 */ altMask, "\033[19;3~", 0, 0},
    {XK_F9, XK_NO_MOD, "\033[20~", 0, 0},
    {XK_F9, /* F21 */ shiftMask, "\033[20;2~", 0, 0},
    {XK_F9, /* F33 */ ctrlMask, "\033[20;5~", 0, 0},
    {XK_F9, /* F45 */ superMask, "\033[20;6~", 0, 0},
    {XK_F9, /* F57 */ altMask, "\033[20;3~", 0, 0},
    {XK_F10, XK_NO_MOD, "\033[21~", 0, 0},
    {XK_F10, /* F22 */ shiftMask, "\033[21;2~", 0, 0},
    {XK_F10, /* F34 */ ctrlMask, "\033[21;5~", 0, 0},
    {XK_F10, /* F46 */ superMask, "\033[21;6~", 0, 0},
    {XK_F10, /* F58 */ altMask, "\033[21;3~", 0, 0},
    {XK_F11, XK_NO_MOD, "\033[23~", 0, 0},
    {XK_F11, /* F23 */ shiftMask, "\033[23;2~", 0, 0},
    {XK_F11, /* F35 */ ctrlMask, "\033[23;5~", 0, 0},
    {XK_F11, /* F47 */ superMask, "\033[23;6~", 0, 0},
    {XK_F11, /* F59 */ altMask, "\033[23;3~", 0, 0},
    {XK_F12, XK_NO_MOD, "\033[24~", 0, 0},
    {XK_F12, /* F24 */ shiftMask, "\033[24;2~", 0, 0},
    {XK_F12, /* F36 */ ctrlMask, "\033[24;5~", 0, 0},
    {XK_F12, /* F48 */ superMask, "\033[24;6~", 0, 0},
    {XK_F12, /* F60 */ altMask, "\033[24;3~", 0, 0},
    {XK_F13, XK_NO_MOD, "\033[1;2P", 0, 0},
    {XK_F14, XK_NO_MOD, "\033[1;2Q", 0, 0},
    {XK_F15, XK_NO_MOD, "\033[1;2R", 0, 0},
    {XK_F16, XK_NO_MOD, "\033[1;2S", 0, 0},
    {XK_F17, XK_NO_MOD, "\033[15;2~", 0, 0},
    {XK_F18, XK_NO_MOD, "\033[17;2~", 0, 0},
    {XK_F19, XK_NO_MOD, "\033[18;2~", 0, 0},
    {XK_F20, XK_NO_MOD, "\033[19;2~", 0, 0},
    {XK_F21, XK_NO_MOD, "\033[20;2~", 0, 0},
    {XK_F22, XK_NO_MOD, "\033[21;2~", 0, 0},
    {XK_F23, XK_NO_MOD, "\033[23;2~", 0, 0},
    {XK_F24, XK_NO_MOD, "\033[24;2~", 0, 0},
    {XK_F25, XK_NO_MOD, "\033[1;5P", 0, 0},
    {XK_F26, XK_NO_MOD, "\033[1;5Q", 0, 0},
    {XK_F27, XK_NO_MOD, "\033[1;5R", 0, 0},
    {XK_F28, XK_NO_MOD, "\033[1;5S", 0, 0},
    {XK_F29, XK_NO_MOD, "\033[15;5~", 0, 0},
    {XK_F30, XK_NO_MOD, "\033[17;5~", 0, 0},
    {XK_F31, XK_NO_MOD, "\033[18;5~", 0, 0},
    {XK_F32, XK_NO_MOD, "\033[19;5~", 0, 0},
    {XK_F33, XK_NO_MOD, "\033[20;5~", 0, 0},
    {XK_F34, XK_NO_MOD, "\033[21;5~", 0, 0},
    {XK_F35, XK_NO_MOD, "\033[23;5~", 0, 0},
};

/*
 * Selection types' masks.
 * Use the same masks as usual.
 * Button1Mask is always unset, to make masks match between ButtonPress.
 * ButtonRelease and MotionNotify.
 * If no match is found, regular selection is used.
 */
static uint selmasks[] = {
    [SEL_RECTANGULAR] = altMask,
};

/*
 * Printable characters in ASCII, used to estimate the advance width
 * of single wide characters.
 */
static char ascii_printable[] = " !\"#$%&'()*+,-./0123456789:;<=>?"
                                "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                "`abcdefghijklmnopqrstuvwxyz{|}~";
