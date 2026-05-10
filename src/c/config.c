#include "config.h"

// If we need a non-backwards compatible version of settings
// check SETTINGS_VERSION_KEY and migrate
#define SETTINGS_VERSION_KEY 1

static AppConfig *s_config;
static Layer *s_layer;

void config_load(AppConfig *config) {
  // default settings, in case the keys don't exist
  config->bg1 = GColorOxfordBlue;
  config->bg2 = GColorLiberty;
  config->date1 = GColorWhite;
  config->date2 = GColorPastelYellow;
  config->digits = GColorPastelYellow;
  config->hour = GColorCeleste;
  config->minute = GColorRajah;
  config->second = GColorPastelYellow;
  config->second_style = SECOND_STYLE_NONE;
  config->update_rate = UPDATE_RATE_MINUTE;

  // Load colors from storage if available
  if (persist_exists(MESSAGE_KEY_BG1)) {
    config->bg1.argb = persist_read_int(MESSAGE_KEY_BG1);
  }

  if (persist_exists(MESSAGE_KEY_BG2)) {
    config->bg2.argb = persist_read_int(MESSAGE_KEY_BG2);
  }

  if (persist_exists(MESSAGE_KEY_DATE1)) {
    config->date1.argb = persist_read_int(MESSAGE_KEY_DATE1);
  }

  if (persist_exists(MESSAGE_KEY_DATE2)) {
    config->date2.argb = persist_read_int(MESSAGE_KEY_DATE2);
  }

  if (persist_exists(MESSAGE_KEY_DIGITS)) {
    config->digits.argb = persist_read_int(MESSAGE_KEY_DIGITS);
  }

  if (persist_exists(MESSAGE_KEY_HOUR)) {
    config->hour.argb = persist_read_int(MESSAGE_KEY_HOUR);
  }

  if (persist_exists(MESSAGE_KEY_MINUTE)) {
    config->minute.argb = persist_read_int(MESSAGE_KEY_MINUTE);
  }

  if (persist_exists(MESSAGE_KEY_SECOND)) {
    config->second.argb = persist_read_int(MESSAGE_KEY_SECOND);
  }

  if (persist_exists(MESSAGE_KEY_SECOND_STYLE)) {
    config->second_style = persist_read_int(MESSAGE_KEY_SECOND_STYLE);
  }

  if (persist_exists(MESSAGE_KEY_UPDATE_RATE)) {
    config->update_rate = persist_read_int(MESSAGE_KEY_UPDATE_RATE);
  }
}

void config_save(AppConfig *config) {
  persist_write_int(SETTINGS_VERSION_KEY, 1);
  persist_write_int(MESSAGE_KEY_BG1, config->bg1.argb);
  persist_write_int(MESSAGE_KEY_BG2, config->bg2.argb);
  persist_write_int(MESSAGE_KEY_DATE1, config->date1.argb);
  persist_write_int(MESSAGE_KEY_DATE2, config->date2.argb);
  persist_write_int(MESSAGE_KEY_DIGITS, config->digits.argb);
  persist_write_int(MESSAGE_KEY_HOUR, config->hour.argb);
  persist_write_int(MESSAGE_KEY_MINUTE, config->minute.argb);
  persist_write_int(MESSAGE_KEY_SECOND, config->second.argb);
  persist_write_int(MESSAGE_KEY_SECOND_STYLE, config->second_style);
  persist_write_int(MESSAGE_KEY_UPDATE_RATE, config->update_rate);
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple* bg1 = dict_find(iter, MESSAGE_KEY_BG1);
  Tuple* bg2 = dict_find(iter, MESSAGE_KEY_BG2);
  Tuple* date1 = dict_find(iter, MESSAGE_KEY_DATE1);
  Tuple* date2 = dict_find(iter, MESSAGE_KEY_DATE2);
  Tuple* digits = dict_find(iter, MESSAGE_KEY_DIGITS);
  Tuple* hour = dict_find(iter, MESSAGE_KEY_HOUR);
  Tuple* minute = dict_find(iter, MESSAGE_KEY_MINUTE);
  Tuple* second = dict_find(iter, MESSAGE_KEY_SECOND);
  Tuple* second_style = dict_find(iter, MESSAGE_KEY_SECOND_STYLE);
  Tuple* update_rate = dict_find(iter, MESSAGE_KEY_UPDATE_RATE);

  if (bg1) {
    s_config->bg1 = GColorFromHEX(bg1->value->int32);
  }
  if (bg2) {
    s_config->bg2 = GColorFromHEX(bg2->value->int32);
  }
  if (date1) {
    s_config->date1 = GColorFromHEX(date1->value->int32);
  }
  if (date2) {
    s_config->date2 = GColorFromHEX(date2->value->int32);
  }
  if (digits) {
    s_config->digits = GColorFromHEX(digits->value->int32);
  }
  if (hour) {
    s_config->hour = GColorFromHEX(hour->value->int32);
  }
  if (minute) {
    s_config->minute = GColorFromHEX(minute->value->int32);
  }
  if (second) {
    s_config->second = GColorFromHEX(second->value->int32);
  }
  if (second_style) {
    s_config->second_style = second_style->value->cstring[0];
  }
  if (update_rate) {
    s_config->update_rate = update_rate->value->cstring[0];
  }

  config_save(s_config);

  layer_mark_dirty(s_layer);
}

void messaging_init(AppConfig *config, Layer *layer) {
  s_config = config;
  s_layer = layer;

  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(app_message_inbox_size_maximum(),
                   app_message_outbox_size_maximum());
}

void messaging_deinit() {
  app_message_deregister_callbacks();
}

