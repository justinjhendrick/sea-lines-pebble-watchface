#include "config.h"

static AppConfig *s_config;
static Layer *s_layer;

void config_load(AppConfig *config) {
  // Set default colors
  config->bg1 = GColorOxfordBlue;
  config->bg2 = GColorLiberty;
  config->date1 = GColorWhite;
  config->date2 = GColorPastelYellow;
  config->digits = GColorPastelYellow;
  config->hour = GColorCeleste;
  config->min = GColorRajah;

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

  if (persist_exists(MESSAGE_KEY_MIN)) {
    config->min.argb = persist_read_int(MESSAGE_KEY_MIN);
  }
}

void config_save(AppConfig *config) {
  persist_write_int(MESSAGE_KEY_BG1, config->bg1.argb);
  persist_write_int(MESSAGE_KEY_BG2, config->bg2.argb);
  persist_write_int(MESSAGE_KEY_DATE1, config->date1.argb);
  persist_write_int(MESSAGE_KEY_DATE2, config->date2.argb);
  persist_write_int(MESSAGE_KEY_DIGITS, config->digits.argb);
  persist_write_int(MESSAGE_KEY_HOUR, config->hour.argb);
  persist_write_int(MESSAGE_KEY_MIN, config->min.argb);
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple *bg1_t = dict_find(iter, MESSAGE_KEY_BG1);
  Tuple *bg2_t = dict_find(iter, MESSAGE_KEY_BG2);
  Tuple *date1_t = dict_find(iter, MESSAGE_KEY_DATE1);
  Tuple *date2_t = dict_find(iter, MESSAGE_KEY_DATE2);
  Tuple *digits_t = dict_find(iter, MESSAGE_KEY_DIGITS);
  Tuple *hour_t = dict_find(iter, MESSAGE_KEY_HOUR);
  Tuple *min_t = dict_find(iter, MESSAGE_KEY_MIN);

  if (bg1_t) {
    s_config->bg1 = GColorFromHEX(bg1_t->value->int32);
  }
  if (bg2_t) {
    s_config->bg2 = GColorFromHEX(bg2_t->value->int32);
  }
  if (date1_t) {
    s_config->date1 = GColorFromHEX(date1_t->value->int32);
  }
  if (date2_t) {
    s_config->date2 = GColorFromHEX(date2_t->value->int32);
  }
  if (digits_t) {
    s_config->digits = GColorFromHEX(digits_t->value->int32);
  }
  if (hour_t) {
    s_config->hour = GColorFromHEX(hour_t->value->int32);
  }
  if (min_t) {
    s_config->min = GColorFromHEX(min_t->value->int32);
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

