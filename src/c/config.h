#pragma once

#include <pebble.h>

typedef struct {
  GColor bg1;
  GColor bg2;
  GColor date1;
  GColor date2;
  GColor digits;
  GColor hour;
  GColor minute;
} AppConfig;

void config_load(AppConfig *config);
void config_save(AppConfig *config);
void messaging_init(AppConfig *config, Layer *layer);
void messaging_deinit();

