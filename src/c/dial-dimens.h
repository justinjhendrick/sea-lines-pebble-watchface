#include <pebble.h>

/* This file defines some constant sizes
 * used to draw the elements of the dial
 */

#if PBL_DISPLAY_WIDTH >= 200
    #define DIMEN_VCR_INSET PBL_IF_ROUND_ELSE(10, 6)
#else
    #define DIMEN_VCR_INSET PBL_IF_ROUND_ELSE(4, 2)
#endif

/* --- Background --- */
#if PBL_DISPLAY_WIDTH >= 260
    #define DIMEN_BG_STRIPE_WIDTH 8
#elif PBL_DISPLAY_WIDTH >= 200
    #define DIMEN_BG_STRIPE_WIDTH 6
#else
    #define DIMEN_BG_STRIPE_WIDTH 4
#endif

/* --- Ticks --- */
#if PBL_DISPLAY_WIDTH >= 260
    #define DIMEN_DATE_SIZE GSize(28, 30)
    #define DIMEN_WDAY_SIZE GSize(46, 30)

    #define DIMEN_TICK_TEXT_SIZE 26
    #define DIMEN_TICK_STROKE_WIDTH 5
#elif PBL_DISPLAY_WIDTH >= 200
    #define DIMEN_DATE_SIZE GSize(28, 30)
    #define DIMEN_WDAY_SIZE GSize(46, 30)

    #define DIMEN_TICK_TEXT_SIZE 22
    #define DIMEN_TICK_STROKE_WIDTH 3
#else
    #define DIMEN_DATE_SIZE GSize(20, 24)
    #define DIMEN_WDAY_SIZE GSize(28, 24)

    #define DIMEN_TICK_TEXT_SIZE 18
    #define DIMEN_TICK_STROKE_WIDTH 3
#endif

/* --- Hands --- */
#if PBL_DISPLAY_WIDTH >= 260
    #define DIMEN_HOUR_STROKE_WIDTH 7
    #define DIMEN_HOUR_TAIL_LENGTH -16
    #define DIMEN_HOUR_BULB_RADIUS 10

    #define DIMEN_MINUTE_STROKE_WIDTH 7
    #define DIMEN_MINUTE_TAIL_LENGTH -12
    #define DIMEN_MINUTE_BULB_RADIUS 7
#elif PBL_DISPLAY_WIDTH >= 200
    #define DIMEN_HOUR_STROKE_WIDTH 7
    #define DIMEN_HOUR_TAIL_LENGTH -14
    #define DIMEN_HOUR_BULB_RADIUS 8

    #define DIMEN_MINUTE_STROKE_WIDTH 7
    #define DIMEN_MINUTE_TAIL_LENGTH -11
    #define DIMEN_MINUTE_BULB_RADIUS 5
#else
    #define DIMEN_HOUR_STROKE_WIDTH 5
    #define DIMEN_HOUR_TAIL_LENGTH -10
    #define DIMEN_HOUR_BULB_RADIUS 6

    #define DIMEN_MINUTE_STROKE_WIDTH 5
    #define DIMEN_MINUTE_TAIL_LENGTH -8
    #define DIMEN_MINUTE_BULB_RADIUS 4
#endif
