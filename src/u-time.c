#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;

#define GColorForPBLColor (GColor8){ .argb = 0b11001100 }

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char time_buffer[] = "00:00";

  // Write the current hours and minutes into the time_buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // strcat(time_buffer,"\n#");
  static char str[20] = "#date ";
  strcpy(str,"#date\n");
  strcat(str,time_buffer);
  strcat(str,"\n#");

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, str);
}

static void main_window_load(Window *window) {


  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  #ifdef PBL_COLOR
    text_layer_set_text_color(s_time_layer, GColorForPBLColor);
  #else
    text_layer_set_text_color(s_time_layer, GColorWhite);
  #endif
  text_layer_set_overflow_mode(s_time_layer,GTextOverflowModeWordWrap);
  text_layer_set_text(s_time_layer, "00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {



  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}