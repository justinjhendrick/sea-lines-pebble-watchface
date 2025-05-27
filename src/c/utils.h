#include <pebble.h>

static GPoint cartesian_from_polar_trigangle(GPoint center, int radius, int trigangle) {
  GPoint ret = {
    .x = (int16_t)(sin_lookup(trigangle) * radius / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(trigangle) * radius / TRIG_MAX_RATIO) + center.y,
  };
  return ret;
}

static GPoint cartesian_from_polar(GPoint center, int radius, int angle_deg) {
  return cartesian_from_polar_trigangle(center, radius, DEG_TO_TRIGANGLE(angle_deg));
}

static GRect rect_from_midpoint(GPoint midpoint, GSize size) {
  GRect ret;
  ret.origin.x = midpoint.x - size.w / 2;
  ret.origin.y = midpoint.y - size.h / 2;
  ret.size = size;
  return ret;
}

static int min(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

static int max(int a, int b) {
  if (a > b) {
    return a;
  }
  return b;
}

static void fast_forward_time(struct tm* now) {
  now->tm_min = now->tm_sec;           /* Minutes. [0-59] */
  now->tm_hour = now->tm_sec % 24;     /* Hours.  [0-23] */
  now->tm_mday = now->tm_sec % 31 + 1; /* Day. [1-31] */
  now->tm_mon = now->tm_sec % 12;      /* Month. [0-11] */
  now->tm_wday = now->tm_sec % 7;      /* Day of week. [0-6] */
}

static int deg_from_mins(int mins) {
  return mins * 360 / 60;
}

static uint32_t trigangle_from_mins(int mins) {
  return mins * TRIG_MAX_ANGLE / 60;
}

static void format_hour(char* buffer, int buffer_len, struct tm* now, bool force_12h) {
  int hour = now->tm_hour;
  if (force_12h || !clock_is_24h_style()) {
    hour = now->tm_hour % 12;
    if (hour == 0) {
      hour = 12;
    }
  }
  snprintf(buffer, buffer_len, "%d", hour);
}

static void format_day_of_week(char* buffer, int buffer_len, struct tm* now) {
  strftime(buffer, buffer_len, "%a", now);
}

static void format_day_th(char* buffer, int buffer_len, struct tm* now) {
  if (now->tm_mday / 10 == 1) {
    strftime(buffer, buffer_len, "%eth", now);
  } else if (now->tm_mday % 10 == 1) {
    strftime(buffer, buffer_len, "%est", now);
  } else if (now->tm_mday % 10 == 2) {
    strftime(buffer, buffer_len, "%end", now);
  } else if (now->tm_mday % 10 == 3) {
    strftime(buffer, buffer_len, "%erd", now);
  } else {
    strftime(buffer, buffer_len, "%eth", now);
  }
}

static void format_day(char* buffer, int buffer_len, struct tm* now) {
  strftime(buffer, buffer_len, "%e", now);
}

static void format_short_month(char* buffer, int buffer_len, struct tm* now) {
  strftime(buffer, buffer_len, "%b", now);
}

static void draw_text_valign(GContext* ctx, const char* buffer, GRect bbox, GTextAlignment align, bool bold, int valign) {
  int h = bbox.size.h;
  int font_height = 0;
  int top_pad = 0;
  GFont font;
  if (h < 14) {
    return;
  } else if (h < 18) {
    font_height = 9;
    top_pad = 4;
    if (bold) {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
    } else {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    }
  } else if (h < 24) {
    font_height = 11;
    top_pad = 6;
    if (bold) {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    } else {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
    }
  } else if (h < 28) {
    font_height = 14;
    top_pad = 9;
    if (bold) {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    } else {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
    }
  } else {
    font_height = 18;
    top_pad = 9;
    if (bold) {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
    } else {
      font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
    }
  }
  int bot_pad = h - font_height - top_pad;
  int shift_up = (top_pad - bot_pad) / 2 + 1;
  if (valign == 1) {
    shift_up = top_pad;
  } else if (valign == 2) {
    shift_up = -bot_pad;
  } else if (valign == 3) {
    shift_up = 0;
  }
  GRect fixed_bbox = GRect(bbox.origin.x, bbox.origin.y - shift_up, bbox.size.w, bbox.size.h);
  graphics_draw_text(ctx, buffer, font, fixed_bbox, GTextOverflowModeWordWrap, align, NULL);
}

static void draw_text_midalign(GContext* ctx, const char* buffer, GRect bbox, GTextAlignment align, bool bold) {
  draw_text_valign(ctx, buffer, bbox, align, bold, 0);
}

static void draw_text_topalign(GContext* ctx, const char* buffer, GRect bbox, GTextAlignment align, bool bold) {
  draw_text_valign(ctx, buffer, bbox, align, bold, 1);
}

static void draw_text_botalign(GContext* ctx, const char* buffer, GRect bbox, GTextAlignment align, bool bold) {
  draw_text_valign(ctx, buffer, bbox, align, bold, 2);
}

static void draw_text_noalign(GContext* ctx, const char* buffer, GRect bbox, GTextAlignment align, bool bold) {
  draw_text_valign(ctx, buffer, bbox, align, bold, 3);
}

static void swap(GPoint* a, GPoint* b) {
  GPoint tmp = *a;
  *a = *b;
  *b = tmp;
}

static void draw_one_digit(
  GContext* ctx,
  int digit,
  GPoint center,
  int inner_radius,
  int outer_radius,
  int left_angle,
  int right_angle,
  bool flip
) {
  GPoint bl = cartesian_from_polar_trigangle(center, inner_radius, left_angle);
  GPoint bc = cartesian_from_polar_trigangle(center, inner_radius, (left_angle + right_angle) / 2);
  GPoint br = cartesian_from_polar_trigangle(center, inner_radius, right_angle);

  GPoint ml = cartesian_from_polar_trigangle(center, (inner_radius + outer_radius) / 2, left_angle);
  GPoint mc = cartesian_from_polar_trigangle(center, (inner_radius + outer_radius) / 2, (left_angle + right_angle) / 2);
  GPoint mr = cartesian_from_polar_trigangle(center, (inner_radius + outer_radius) / 2, right_angle);

  GPoint tl = cartesian_from_polar_trigangle(center, outer_radius, left_angle);
  GPoint tc = cartesian_from_polar_trigangle(center, outer_radius, (left_angle + right_angle) / 2);
  GPoint tr = cartesian_from_polar_trigangle(center, outer_radius, right_angle);

  if (flip) {
    swap(&bl, &tr);
    swap(&bc, &tc);
    swap(&br, &tl);
    swap(&mr, &ml);
  }

  if (digit == 0) {
    graphics_draw_line(ctx, tl, tr);
    graphics_draw_line(ctx, tr, br);
    graphics_draw_line(ctx, br, bl);
    graphics_draw_line(ctx, bl, tl);
  } else if (digit == 1) {
    graphics_draw_line(ctx, tc, bc);
  } else if (digit == 2) {
    graphics_draw_line(ctx, tl, tr);
    graphics_draw_line(ctx, tr, mr);
    graphics_draw_line(ctx, mr, ml);
    graphics_draw_line(ctx, ml, bl);
    graphics_draw_line(ctx, bl, br);
  } else if (digit == 3) {
    graphics_draw_line(ctx, tl, tr);
    graphics_draw_line(ctx, tr, br);
    graphics_draw_line(ctx, mr, ml);
    graphics_draw_line(ctx, br, bl);
  } else if (digit == 4) {
    graphics_draw_line(ctx, tl, ml);
    graphics_draw_line(ctx, ml, mr);
    graphics_draw_line(ctx, tr, br);
  } else if (digit == 5) {
    graphics_draw_line(ctx, tl, tr);
    graphics_draw_line(ctx, tl, ml);
    graphics_draw_line(ctx, ml, mr);
    graphics_draw_line(ctx, mr, br);
    graphics_draw_line(ctx, br, bl);
  } else if (digit == 6) {
    graphics_draw_line(ctx, tr, tl);
    graphics_draw_line(ctx, tl, bl);
    graphics_draw_line(ctx, bl, br);
    graphics_draw_line(ctx, br, mr);
    graphics_draw_line(ctx, mr, ml);
  } else if (digit == 7) {
    graphics_draw_line(ctx, tl, tr);
    graphics_draw_line(ctx, tr, br);
  } else if (digit == 8) {
    graphics_draw_line(ctx, tl, tr);
    graphics_draw_line(ctx, ml, mr);
    graphics_draw_line(ctx, bl, br);
    graphics_draw_line(ctx, tl, bl);
    graphics_draw_line(ctx, tr, br);
  } else if (digit == 9) {
    graphics_draw_line(ctx, mr, ml);
    graphics_draw_line(ctx, ml, tl);
    graphics_draw_line(ctx, tl, tr);
    graphics_draw_line(ctx, tr, br);
    graphics_draw_line(ctx, br, bl);
  }
}