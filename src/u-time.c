#include <pebble.h>

#define TEST_DRIVER true
#define GColorForPBLColor (GColor8){ .argb = 0b11001100 }

  
static Window *s_main_window;
static TextLayer *s_time_layer;
static char window_text[30];
static int type_count_clear = 0;
static int type_count_date = 0;



static void update_text_layer(){
  text_layer_set_text(s_time_layer, window_text);
}


static void clear_text_layer(){
  strcpy(window_text,"");
  update_text_layer();
}


static void add_text(char* text_append){
  strcat(window_text,text_append);
  update_text_layer();
}


static void type_cmd_date(){
  type_count_date++;
  APP_LOG(APP_LOG_LEVEL_DEBUG,"%s%i", "type_cmd_date, called with type_count_date=",type_count_date);

  if(type_count_date==1){
    add_text(">d");
  } else if(type_count_date==2){
    add_text("a"); 
  } else if(type_count_date==3){
    add_text("t");   
  } else if(type_count_date==4){
    add_text("e");    
  } else{
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);
    static char time_buffer[] = "00:00";
  
    // Write the current hours and minutes into the time_buffer
    if(clock_is_24h_style() == true) {
      //Use 2h hour format
      strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
      //Use 12 hour format
      strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    add_text("\n");
    add_text(time_buffer);
    add_text("\n>");
    type_count_date=0;
  }
  
  if(type_count_date!=0){
    app_timer_register(200, type_cmd_date, NULL);
  }

}



static void type_cmd_clear(){
  type_count_clear++;
  APP_LOG(APP_LOG_LEVEL_DEBUG,"type_cmd_clear, called with type_count_clear=%i, window_text=%s",type_count_clear,window_text);
  if(type_count_clear==1){
    add_text("c");
  } else if(type_count_clear==2){
    add_text("l");    
  } else if(type_count_clear==3){
    add_text("e");  
  } else if(type_count_clear==4){
    add_text("a");  
  } else if(type_count_clear==5){
    add_text("r");      
  } else{
    clear_text_layer();
    type_count_clear=0;
     app_timer_register(200, type_cmd_date, NULL);
  }
  
  if(type_count_clear!=0){
    app_timer_register(200, type_cmd_clear, NULL);
  }

}




static void update_time() {
  // Get a tm structure
  // time_t temp = time(NULL); 
  // struct tm *tick_time = localtime(&temp);

  // // Create a long-lived buffer
  // static char time_buffer[] = "00:00";

  // // Write the current hours and minutes into the time_buffer
  // if(clock_is_24h_style() == true) {
  //   //Use 2h hour format
  //   strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  // } else {
  //   //Use 12 hour format
  //   strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  // }

  
  type_cmd_clear();
  // add_text("clear");
  // psleep(500);
  // clear_text_layer();
  // add_text("");
  // psleep(50);
  // add_text("date\n");
  // add_text(time_buffer);
  // if(tick_time->tm_sec % 2 == 0)
  //   add_text("\n");

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

  #if TEST_DRIVER
   if(tick_time->tm_sec % 5 == 0)
     update_time();
  #else
    update_time(); 
  #endif

  
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
  // tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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