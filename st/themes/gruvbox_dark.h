const char *colorname[] = {
    /* 8 normal colors */
    [0] = "#928374",
    [1] = "#fb4934",
    [2] = "#b8bb26",
    [3] = "#fabd2f",
    [4] = "#83a598",
    [5] = "#d3869b",
    [6] = "#8ec07c",
    [7] = "#ebdbb2",

    /* 8 bright colors */
    [8] = "#282828",
    [9] = "#cc241d",
    [10] = "#98971a",
    [11] = "#d79921",
    [12] = "#458588",
    [13] = "#b16286",
    [14] = "#689d6a",
    [15] = "#a89984",

    [256] = "#282828", /* background */
    [257] = "#EBDBB2", /* foreground */
    [258] = "#665C54", /* cursor */
};

/*
 * Default colors (colorname index)
 * foreground, background, cursor, reverse cursor
 */

unsigned int defaultbg = 256;
unsigned int defaultfg = 257;
unsigned int defaultcs = 258;
unsigned int defaultrcs = 258;
