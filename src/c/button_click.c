#include <pebble.h>

static Window *window;
static TextLayer *header_layer,*text_layer;
static ActionBarLayer *a_bar;
static GBitmap *plus_icon, *minus_icon, *reset_icon;

static int number_items = 0;
static void update_text(){
  static char body_text[18];
  snprintf(body_text, sizeof(body_text), "%u Items", number_items);
  text_layer_set_text(text_layer, body_text);
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  number_items = 0;
  update_text();
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  number_items ++;
  update_text();
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (number_items != 0)
  {
    number_items --;
    update_text();
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  a_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(a_bar, window);
  action_bar_layer_set_click_config_provider(a_bar, click_config_provider);

  action_bar_layer_set_icon(a_bar, BUTTON_ID_UP, plus_icon);
  action_bar_layer_set_icon(a_bar, BUTTON_ID_SELECT, reset_icon);
  action_bar_layer_set_icon(a_bar, BUTTON_ID_DOWN, minus_icon);
  
  int width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;
  
  header_layer = text_layer_create(GRect(4, PBL_IF_RECT_ELSE(0, 30), width, 60));
  text_layer_set_font(header_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(header_layer, GColorClear);
  text_layer_set_text(header_layer, "Item Counter");
  text_layer_set_text_alignment(header_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  layer_add_child(window_layer, text_layer_get_layer(header_layer));
  
  text_layer = text_layer_create(GRect(3, PBL_IF_RECT_ELSE(44, 60) + 28, width, 60));
  text_layer_set_text(text_layer, "0 items");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(header_layer);
  text_layer_destroy(text_layer);
}

static void init(void) {
  plus_icon = gbitmap_create_with_resource(RESOURCE_ID_plus_icon);
  minus_icon = gbitmap_create_with_resource(RESOURCE_ID_minus_icon);
  reset_icon = gbitmap_create_with_resource(RESOURCE_ID_reset_icon);
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
  gbitmap_destroy(plus_icon);
  gbitmap_destroy(minus_icon);
  gbitmap_destroy(reset_icon);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}