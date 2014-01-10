/*

  Polar Clock watch (SDK 2.0)

  Thanks to Team Pebble's Segment Six watchface...it was a big help!

 */

#include "pebble.h"

#define SHOW_TEXT_TIME 1
#define SHOW_TEXT_DATE 1
#define ROW_DATE 0
#define TWENTY_FOUR_HOUR_DIAL 0
	
#define BACKGROUND_COLOR GColorBlack
#define FOREGROUND_COLOR GColorWhite

Window *window;

Layer *minute_display_layer;
Layer *hour_display_layer;
Layer *second_display_layer;

#if SHOW_TEXT_TIME
TextLayer *text_time_layer;
#endif

#if SHOW_TEXT_DATE
TextLayer *text_date_layer;
#endif

const GPathInfo SECOND_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-7, -70}, // 70 = radius + fudge; 7 = 70*tan(6 degrees); 6 degrees per minute;
    {7,  -70},
  }
};

static GPath *second_segment_path;


const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-6, -58}, // 58 = radius + fudge; 6 = 58*tan(6 degrees); 30 degrees per hour;
    {6,  -58},
  }
};

static GPath *minute_segment_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-5, -48}, // 48 = radius + fudge; 5 = 48*tan(6 degrees); 6 degrees per second;
    {5,  -48},
  }
};

static GPath *hour_segment_path;


static void second_display_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle = (t->tm_sec + 1) * 6;

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

  graphics_fill_circle(ctx, center, 65);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(second_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, second_segment_path);

  }

  graphics_fill_circle(ctx, center, 60);

}


static void minute_display_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle = t->tm_min * 6;

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

  graphics_fill_circle(ctx, center, 55);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, minute_segment_path);

  }

  graphics_fill_circle(ctx, center, 50);

}


static void hour_display_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle;

  #if TWENTY_FOUR_HOUR_DIAL
    angle = (t->tm_hour * 15) + (t->tm_min / 4);
  #else
    angle = (( t->tm_hour % 12 ) * 30) + (t->tm_min / 2);
  #endif

  angle = angle - (angle % 6);

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

  graphics_fill_circle(ctx, center, 45);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(hour_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, hour_segment_path);
  }

  graphics_fill_circle(ctx, center, 40);
}


static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  //(void)ctx;
	
  layer_mark_dirty(second_display_layer);
  layer_mark_dirty(minute_display_layer);
  layer_mark_dirty(hour_display_layer);

  #if SHOW_TEXT_TIME

  // Need to be static because it's used by the system later.
  static char time_text[] = "00:00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  text_layer_set_text(text_time_layer, time_text);

  #endif

  #if SHOW_TEXT_DATE
  
    #if ROW_DATE
    static char date_text[] = "00 Xxx";
    strftime(date_text, sizeof(date_text), "%e %b", tick_time);
    #else
    static char date_text[] = "Xxx 00";
    strftime(date_text, sizeof(date_text), "%b %e", tick_time);
    #endif

  text_layer_set_text(text_date_layer, date_text);

  #endif

}

 void in_received_handler(DictionaryIterator *received, void *context) {
   // incoming message received
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

static void init(void) {
  //(void)ctx;
	
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_open(64, 0);

  window = window_create();
  window_set_background_color(window, BACKGROUND_COLOR);
  window_stack_push(window, true);

  // No longer needed for SDK 2.0
  //resource_init_current_app(&APP_RESOURCES);
	
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Init the layer for the second display
  second_display_layer = layer_create(bounds);
  layer_set_update_proc(second_display_layer, second_display_layer_update_callback);
  layer_add_child(window_layer, second_display_layer);

  // Init the second segment path
  second_segment_path = gpath_create(&SECOND_SEGMENT_PATH_POINTS);
  gpath_move_to(second_segment_path, grect_center_point(&bounds));
	
  // Init the layer for the minute display
  minute_display_layer = layer_create(bounds);
  layer_set_update_proc(minute_display_layer, minute_display_layer_update_callback);
  layer_add_child(window_layer, minute_display_layer);

  // Init the minute segment path
  minute_segment_path = gpath_create(&MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(minute_segment_path, grect_center_point(&bounds));

  // Init the layer for the hour display
  hour_display_layer = layer_create(bounds);
  layer_set_update_proc(hour_display_layer, hour_display_layer_update_callback);
  layer_add_child(window_layer, hour_display_layer);

  // Init the hour segment path
  hour_segment_path = gpath_create(&HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(hour_segment_path, grect_center_point(&bounds));

  #if SHOW_TEXT_TIME

  text_time_layer = text_layer_create(bounds);
  //text_layer_init(text_time_layer, window.layer.frame);
  text_layer_set_text_color(text_time_layer, FOREGROUND_COLOR);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);

  #if SHOW_TEXT_DATE
  //layer_set_frame(text_layer_get_layer(text_time_layer), GRect(47, 57, 144-47, 168-57));
  layer_set_frame(text_layer_get_layer(text_time_layer), GRect(0, 57, 144, 168-57));
  #else
  //layer_set_frame(text_layer_get_layer(text_time_layer), GRect(47, 70, 144-47, 168-70));
  layer_set_frame(text_layer_get_layer(text_time_layer), GRect(0, 70, 144, 168-70));
  #endif

  //text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  #endif

  #if SHOW_TEXT_DATE

  text_date_layer = text_layer_create(bounds);
  //text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(text_date_layer, FOREGROUND_COLOR);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
	
    #if SHOW_TEXT_TIME
    //layer_set_frame(text_layer_get_layer(text_date_layer), GRect(44, 80, 144-44, 168-80));
	layer_set_frame(text_layer_get_layer(text_date_layer), GRect(0, 80, 144, 168-80));
    #else
    //layer_set_frame(text_layer_get_layer(text_date_layer), GRect(44, 70, 144-44, 168-70));
	layer_set_frame(text_layer_get_layer(text_date_layer), GRect(0, 70, 144, 168-70));
    #endif
  //text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  #endif

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
  gpath_destroy(second_segment_path);
  gpath_destroy(minute_segment_path);
  gpath_destroy(hour_segment_path);

  tick_timer_service_unsubscribe();
  window_destroy(window);
  layer_destroy(minute_display_layer);
  layer_destroy(hour_display_layer);
  layer_destroy(second_display_layer);
	
  #if SHOW_TEXT_TIME
  text_layer_destroy(text_time_layer);
  #endif

  #if SHOW_TEXT_DATE
  text_layer_destroy(text_date_layer);
  #endif
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

/*
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
*/