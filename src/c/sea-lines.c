#include <pebble.h>
#include "utils.h"

#define DEBUG_TIME (false)
#define BUFFER_LEN (10)
#define COL_BG1   (GColorOxfordBlue)
#define COL_BG2   (GColorLiberty)
#define COL_HOUR  (GColorCeleste)
#define COL_MIN   (GColorRajah)
#define COL_BEIGE (GColorPastelYellow)

static Window* s_window;
static Layer* s_layer;
static char s_buffer[BUFFER_LEN];

static void draw_bg(GContext* ctx, GRect bounds, GPoint center, int vcr) {
  int width = 4;
  int i = 0;
  int to_corner = 1414 * max(bounds.size.h, bounds.size.w) / 1000;
  for (int radius = to_corner; radius > width; radius -= width) {
    if (i++ % 2 == 0) {
      graphics_context_set_fill_color(ctx, COL_BG1);
    } else {
      graphics_context_set_fill_color(ctx, COL_BG2);
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
      graphics_context_set_text_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, COL_BEIGE);
      GSize date_size = GSize(20, 24);
      GRect date_bbox = rect_from_midpoint(
        cartesian_from_polar_trigangle(center, vcr - date_size.w / 2, angle),
        date_size
      );
      graphics_fill_rect(ctx, date_bbox, 0, GCornerNone);
      format_day(s_buffer, BUFFER_LEN, now);
      draw_text_midalign(ctx, s_buffer, date_bbox, GTextAlignmentCenter, true);

      // day of week
      graphics_context_set_text_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorWhite);
      GSize wday_size = GSize(28, 24);
      GRect wday_bbox = rect_from_midpoint(
        cartesian_from_polar_trigangle(center, vcr - date_size.w - wday_size.w / 2, angle),
        wday_size
      );
      graphics_fill_rect(ctx, wday_bbox, 0, GCornerNone);
      format_day_of_week(s_buffer, BUFFER_LEN, now);
      draw_text_midalign(ctx, s_buffer, wday_bbox, GTextAlignmentCenter, false);
    } else {
      graphics_context_set_stroke_color(ctx, COL_BEIGE);
      graphics_context_set_stroke_width(ctx, 3);
      int text_size = 18;
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
  graphics_context_set_stroke_width(ctx, 5);
  graphics_context_set_fill_color(ctx, COL_HOUR);
  graphics_context_set_stroke_color(ctx, COL_HOUR);
  int total_mins = 12 * 60;
  int current_mins = now->tm_hour * 60 + now->tm_min;
  int angle = current_mins * TRIG_MAX_ANGLE / total_mins;
  graphics_draw_line(
    ctx,
    cartesian_from_polar_trigangle(center, -10, angle),
    cartesian_from_polar_trigangle(center, vcr - 28, angle)
  );
  graphics_fill_circle(ctx, center, 6);
}

static void draw_minute(GContext* ctx, GRect bounds, GPoint center, int vcr, struct tm* now) {
  graphics_context_set_stroke_width(ctx, 5);
  graphics_context_set_stroke_color(ctx, COL_MIN);
  graphics_context_set_fill_color(ctx, COL_MIN);
  int total_mins = 60;
  int current_mins = now->tm_min;
  int angle = current_mins * TRIG_MAX_ANGLE / total_mins;
  graphics_draw_line(
    ctx,
    cartesian_from_polar_trigangle(center, -8, angle),
    cartesian_from_polar_trigangle(center, vcr - 10, angle)
  );
  graphics_fill_circle(ctx, center, 4);
}

static void update_layer(Layer* layer, GContext* ctx) {
  time_t temp = time(NULL);
  struct tm* now = localtime(&temp);
  if (DEBUG_TIME) {
    fast_forward_time(now);
  }

  GRect bounds = layer_get_bounds(layer);
  int vcr = min(bounds.size.h, bounds.size.w) / 2 - 1;
  GPoint center = grect_center_point(&bounds);
  draw_bg(ctx, bounds, center, vcr);
  draw_ticks(ctx, bounds, center, vcr, now);
  draw_hour(ctx, bounds, center, vcr, now);
  draw_minute(ctx, bounds, center, vcr, now);
}

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_window, COL_BG1);
  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, update_layer);
  layer_add_child(window_layer, s_layer);
}

static void window_unload(Window* window) {
  layer_destroy(s_layer);
}

static void tick_handler(struct tm* now, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
}

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(DEBUG_TIME ? SECOND_UNIT : MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}