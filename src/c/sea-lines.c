#include <pebble.h>
#include "utils.h"
#include "config.h"
#include "dial-dimens.h"

#define DEBUG_TIME (false)
#define BUFFER_LEN (10)

static Window* s_window;
static Layer* s_layer;
static char s_buffer[BUFFER_LEN];
static AppConfig s_config;
static TimeUnits s_time_units = MINUTE_UNIT;

static void draw_bg(GContext* ctx, GRect bounds, GPoint center, int vcr) {
  int i = 0;
  int width = DIMEN_BG_STRIPE_WIDTH;
  int to_corner = 1414 * max(bounds.size.h, bounds.size.w) / 1000;
  for (int radius = to_corner; radius > width; radius -= width) {
    if (i++ % 2 == 0) {
      graphics_context_set_fill_color(ctx, s_config.bg1);
    } else {
      graphics_context_set_fill_color(ctx, s_config.bg2);
    }
    graphics_fill_circle(ctx, center, radius);
    GRect r = (GRect) {
      .origin = cartesian_from_polar(center, radius, 0),
      .size = GSize(bounds.size.w / 2, radius * 2 + 1),
    };
    graphics_fill_rect(ctx, r, 0, GCornerNone);
  }
}

static void draw_ticks(GContext* ctx, GRect bounds, GPoint center, int vcr, struct tm* now) {
  for (int hour = 0; hour < 12; hour++) {
    int angle = hour * TRIG_MAX_ANGLE / 12;
    if (hour == 3) {
      // date of month
      graphics_context_set_text_color(ctx, gcolor_legible_over(s_config.date2));
      graphics_context_set_fill_color(ctx, s_config.date2);
      GSize date_size = DIMEN_DATE_SIZE;
      GRect date_bbox = rect_from_midpoint(
        cartesian_from_polar_trigangle(center, vcr - date_size.w / 2, angle),
        date_size
      );
      graphics_fill_rect(ctx, date_bbox, 0, GCornerNone);
      format_day(s_buffer, BUFFER_LEN, now);
      draw_text_midalign(ctx, s_buffer, date_bbox, GTextAlignmentCenter, true);

      // day of week
      graphics_context_set_text_color(ctx, gcolor_legible_over(s_config.date1));
      graphics_context_set_fill_color(ctx, s_config.date1);
      GSize wday_size = DIMEN_WDAY_SIZE;
      GRect wday_bbox = rect_from_midpoint(
        cartesian_from_polar_trigangle(center, vcr - date_size.w - wday_size.w / 2, angle),
        wday_size
      );
      graphics_fill_rect(ctx, wday_bbox, 0, GCornerNone);
      format_day_of_week(s_buffer, BUFFER_LEN, now);
      draw_text_midalign(ctx, s_buffer, wday_bbox, GTextAlignmentCenter, false);
    } else {
      graphics_context_set_stroke_color(ctx, s_config.digits);
      graphics_context_set_stroke_width(ctx, DIMEN_TICK_STROKE_WIDTH);
      int text_size = DIMEN_TICK_TEXT_SIZE;
      int hour_text = (hour == 0) ? 12 : hour;
      int half_width = DEG_TO_TRIGANGLE(8);
      bool flip = (hour_text > 3 && hour_text < 9);
      if (hour_text < 10) {
        draw_one_digit(ctx, hour_text, center, vcr - text_size, vcr, angle - half_width, angle + half_width, flip);
      } else {
        draw_one_digit(ctx, hour_text / 10, center, vcr - text_size, vcr, angle - half_width, angle, flip);
        draw_one_digit(ctx, hour_text % 10, center, vcr - text_size, vcr, angle, angle + half_width, flip);
      }
    }
  }
}

static void draw_hour(GContext* ctx, GRect bounds, GPoint center, int vcr, struct tm* now) {
  graphics_context_set_stroke_width(ctx, DIMEN_HOUR_STROKE_WIDTH);
  graphics_context_set_fill_color(ctx, s_config.hour);
  graphics_context_set_stroke_color(ctx, s_config.hour);
  int total_mins = 12 * 60;
  int current_mins = now->tm_hour * 60 + now->tm_min;
  int angle = current_mins * TRIG_MAX_ANGLE / total_mins;
  graphics_draw_line(
    ctx,
    cartesian_from_polar_trigangle(center, DIMEN_HOUR_TAIL_LENGTH, angle),
    cartesian_from_polar_trigangle(center, 6 * vcr / 10, angle)
  );
  graphics_fill_circle(ctx, center, DIMEN_HOUR_BULB_RADIUS);
}

static void draw_minute(GContext* ctx, GRect bounds, GPoint center, int vcr, struct tm* now) {
  graphics_context_set_stroke_width(ctx, DIMEN_MINUTE_STROKE_WIDTH);
  graphics_context_set_stroke_color(ctx, s_config.minute);
  graphics_context_set_fill_color(ctx, s_config.minute);
  int total_mins = 60;
  int current_mins = now->tm_min;
  int angle = current_mins * TRIG_MAX_ANGLE / total_mins;
  graphics_draw_line(
    ctx,
    cartesian_from_polar_trigangle(center, DIMEN_MINUTE_TAIL_LENGTH, angle),
    cartesian_from_polar_trigangle(center, 9 * vcr / 10, angle)
  );
  graphics_fill_circle(ctx, center, DIMEN_MINUTE_BULB_RADIUS);
}

static void draw_second(GContext* ctx, GRect bounds, GPoint center, int vcr, struct tm* now) {
  graphics_context_set_stroke_color(ctx, s_config.second);
  graphics_context_set_fill_color(ctx, s_config.second);
  int total_sec = 60;
  int current_sec = now->tm_sec;
  int angle = current_sec * TRIG_MAX_ANGLE / total_sec;
  if (s_config.second_style == SECOND_STYLE_DOT) {
    graphics_context_set_stroke_width(ctx, 1);
    int radius = DIMEN_SECOND_DOT_RADIUS;
    graphics_fill_circle(
      ctx,
      cartesian_from_polar_trigangle(center, vcr - DIMEN_TICK_TEXT_SIZE - radius * 2 - 1, angle),
      radius
    );
  } else if (s_config.second_style == SECOND_STYLE_HAND) {
    graphics_context_set_stroke_width(ctx, DIMEN_SECOND_STROKE_WIDTH);
    graphics_draw_line(
      ctx,
      cartesian_from_polar_trigangle(center, DIMEN_SECOND_TAIL_LENGTH, angle),
      cartesian_from_polar_trigangle(center, 9 * vcr / 10, angle)
    );
    graphics_fill_circle(ctx, center, DIMEN_SECOND_BULB_RADIUS);
  }
}

static void tick_handler(struct tm* now, TimeUnits units_changed) {
  if (s_config.update_rate == UPDATE_RATE_5SECOND && now->tm_sec % 5 != 0) {
    // skip redrawing the screen to save battery
  } else {
    layer_mark_dirty(window_get_root_layer(s_window));
  }
}

static void tick_resub() {
  if (s_config.second_style == SECOND_STYLE_NONE && s_time_units != MINUTE_UNIT) {
    // ignore update rate if there is no second hand/dot
    s_time_units = MINUTE_UNIT;
    tick_timer_service_subscribe(s_time_units, tick_handler);
  } else if (s_time_units != SECOND_UNIT) {
    // tick timer only offers per second or per minute.
    // So we use per-second and skip some redraws when in per-5-second mode.
    s_time_units = SECOND_UNIT;
    tick_timer_service_subscribe(s_time_units, tick_handler);
  }
}

static void update_layer(Layer* layer, GContext* ctx) {
  time_t temp = time(NULL);
  struct tm* now = localtime(&temp);
  if (DEBUG_TIME) {
    fast_forward_time(now);
  }

  GRect bounds = layer_get_unobstructed_bounds(layer);
  int vcr = min(bounds.size.h, bounds.size.w) / 2 - DIMEN_VCR_INSET;
  GPoint center = grect_center_point(&bounds);
  draw_bg(ctx, bounds, center, vcr);
  draw_ticks(ctx, bounds, center, vcr, now);
  draw_hour(ctx, bounds, center, vcr, now);
  draw_minute(ctx, bounds, center, vcr, now);
  draw_second(ctx, bounds, center, vcr, now);

  tick_resub();
}

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_window, s_config.bg1);
  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, update_layer);
  layer_add_child(window_layer, s_layer);
}

static void window_unload(Window* window) {
  layer_destroy(s_layer);
}

static void init(void) {
  config_load(&s_config);
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(s_time_units, tick_handler);
  tick_resub();
  messaging_init(&s_config, window_get_root_layer(s_window));
}

static void deinit(void) {
  config_save(&s_config);
  messaging_deinit();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
