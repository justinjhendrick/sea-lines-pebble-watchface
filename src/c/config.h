#pragma once

#include <pebble.h>

// Manually make sure clay config stays in sync
#define SECOND_STYLE_NONE ('N')
#define SECOND_STYLE_DOT ('D')
#define SECOND_STYLE_HAND ('H')

#define UPDATE_RATE_MINUTE ('M')
#define UPDATE_RATE_1SECOND ('1')
#define UPDATE_RATE_5SECOND ('5')

typedef struct {
  GColor bg1;
  GColor bg2;
  GColor date1;
  GColor date2;
  GColor digits;
  GColor hour;
  GColor minute;
  GColor second;
  char second_style;
  char update_rate;
} AppConfig;

void config_load(AppConfig *config);
void config_save(AppConfig *config);
void messaging_init(AppConfig *config, Layer *layer);
void messaging_deinit();

