#include <pebble.h>
#include "utils.h"

#define DEBUG_TIME (false)
#define BUFFER_LEN (100)
#define COL_BG1   (GColorIndigo)
#define COL_BG2   (GColorLiberty)
#define COL_HOUR  (GColorCeleste)
#define COL_MIN   (GColorOrange)
#define COL_BEIGE (GColorLimerick)

static Window* s_window;
static Layer* s_layer;
static char s_buffer[BUFFER_LEN];
static GPath* s_arc;

static const GPathInfo ARC_POINTS = {
  .num_points = 80,
  .points = (GPoint []) {
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
  }
};

static void draw_bg(GContext* ctx, GRect bounds, GPoint center, int vcr) {
  graphics_context_set_fill_color(ctx, COL_BG2);
  int width = 6;
  int HALF = DEG_TO_TRIGANGLE(180);
  int right = bounds.origin.x + bounds.size.w;
  int curve_points = (ARC_POINTS.num_points - 4) / 2 - 1;
  for (int radius = width; radius < 3 * vcr / 2; radius += 2 * width) {
    int i = 0;
    ARC_POINTS.points[i++] = GPoint(right, center.y - radius - width);
    for (int angle = HALF * 2; angle > HALF; angle -= HALF / curve_points) {
      ARC_POINTS.points[i++] = cartesian_from_polar_trigangle(center, radius + width, angle);
    }
    ARC_POINTS.points[i++] = GPoint(right, center.y + radius + width);
    ARC_POINTS.points[i++] = GPoint(right, center.y + radius);
    for (int angle = HALF; angle < 2 * HALF; angle += HALF / curve_points) {
      ARC_POINTS.points[i++] = cartesian_from_polar_trigangle(center, radius, angle);
    }
    ARC_POINTS.points[i++] = GPoint(right, center.y - radius);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d\n", i);
    gpath_draw_filled(ctx, s_arc);
  }
}

static void update_layer(Layer* layer, GContext* ctx) {
  time_t temp = time(NULL);
  struct tm* now = localtime(&temp);
  if (DEBUG_TIME) {
    fast_forward_time(now);
  }

  GRect bounds = layer_get_bounds(layer);
  int vcr = min(bounds.size.h, bounds.size.w) / 2;
  GPoint center = grect_center_point(&bounds);
  draw_bg(ctx, bounds, center, vcr);
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
  s_arc = gpath_create(&ARC_POINTS);
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