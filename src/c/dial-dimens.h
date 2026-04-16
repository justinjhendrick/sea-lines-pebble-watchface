#include <pebble.h>

/* This file defines some constant sizes
 * used to draw the elements of the dial
 */

#define DIMEN_VCR_INSET PBL_IF_ROUND_ELSE(4, 2)

/* --- Background --- */
#define DIMEN_BG_STRIPE_WIDTH 4

/* --- Ticks --- */
#define DIMEN_DATE_SIZE GSize(20, 24)
#define DIMEN_WDAY_SIZE GSize(28, 24)
#define DIMEN_TICK_STROKE_WIDTH 3
#define DIMEN_TICK_TEXT_SIZE 18

/* --- Hands --- */
#define DIMEN_HOUR_STROKE_WIDTH 5
#define DIMEN_HOUR_TAIL_LENGTH -10
#define DIMEN_HOUR_BULB_RADIUS 6

#define DIMEN_MINUTE_STROKE_WIDTH 5
#define DIMEN_MINUTE_TAIL_LENGTH -8
#define DIMEN_MINUTE_BULB_RADIUS 4
