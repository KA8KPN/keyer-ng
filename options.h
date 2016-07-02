#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

// DISPLAY_LARGE means 4x20 display, otherwise it means the system has a small
// display, which is a 2x16 display
#define DISPLAY_LARGE
// #define DISPLAY_SMALL

#if defined(DISPLAY_LARGE) || defined(DISPLAY_SMALL)
#define LCD_DISPLAY
#endif // DISPLAY_LARGE .or. DISPLAY_SMALL

#endif // OPTIONS_H_INCLUDED
