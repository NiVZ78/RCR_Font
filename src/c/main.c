#include <pebble.h>

// Pointer to main window and layer
static Window *s_main_window;
static Layer *s_main_window_layer;


static GBitmap *s_digit_all_images;      // Source bitmap to hold image with ALL DIGIT
static GBitmap *s_digit_image[14];       // Array to hold sub-bitmaps of individual DIGIT images [0-9]


static BitmapLayer *s_hour1_layer;
static BitmapLayer *s_hour2_layer;

static BitmapLayer *s_minute1_layer;
static BitmapLayer *s_minute2_layer;





static void update_hours(struct tm *tick_time) {
      
  bitmap_layer_set_bitmap(s_hour1_layer, s_digit_image[tick_time->tm_hour/10]);
  bitmap_layer_set_bitmap(s_hour2_layer, s_digit_image[tick_time->tm_hour%10]);
  
}



static void update_minutes(struct tm *tick_time) {
      
  bitmap_layer_set_bitmap(s_minute1_layer, s_digit_image[tick_time->tm_min/10]);
  bitmap_layer_set_bitmap(s_minute2_layer, s_digit_image[tick_time->tm_min%10]);
  
}




static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  /* You could just mark the layer dirty and use graphics_draw_bitmap_in_rect
     but this is the code I tend to use based on @Orviwan 91 Dub v2.0
  */
  
  /*
  if (units_changed & MONTH_UNIT ) 
  {
    update_month(tick_time);
  }
  if (units_changed & DAY_UNIT) 
  {
    update_days(tick_time);
  }
  */
  
  if (units_changed & HOUR_UNIT) 
  {
    update_hours(tick_time);
  }
  if (units_changed & MINUTE_UNIT) 
  {
    update_minutes(tick_time);
  }
  
  /*
  if (units_changed & SECOND_UNIT) 
  {
    update_seconds(tick_time);
  }
  */
  
}



// WINDOW LOAD
static void main_window_load(Window *window) {

  
  // Get the root window layer
  s_main_window_layer = window_get_root_layer(s_main_window);

  // Create the bitmap with ALL digits
  s_digit_all_images = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RCR_ALL_DIGITS);  
  
  // Loop through cutting out the individual images and store these in the array
  for (int i=0; i<10; i++){
    s_digit_image[i] = gbitmap_create_as_sub_bitmap(s_digit_all_images, GRect(0+(10*i), 0, 10, 10));
  }
  
  // Create the bitmap layers for the time digits
  s_hour1_layer = bitmap_layer_create(GRect(12, 5, 10, 10));
  s_hour2_layer = bitmap_layer_create(GRect(25, 5, 10, 10));
  s_minute1_layer = bitmap_layer_create(GRect(44, 5, 10, 10));
  s_minute2_layer = bitmap_layer_create(GRect(57, 5, 10, 10));
  
  // Add the layers to the parent layer
  layer_add_child(s_main_window_layer, bitmap_layer_get_layer(s_hour1_layer));
  layer_add_child(s_main_window_layer, bitmap_layer_get_layer(s_hour2_layer));
  layer_add_child(s_main_window_layer, bitmap_layer_get_layer(s_minute1_layer));
  layer_add_child(s_main_window_layer, bitmap_layer_get_layer(s_minute2_layer));
  
  // Force an initial update
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  tick_handler(tick_time, YEAR_UNIT + MONTH_UNIT + DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
  
  // Subscribe to tick handler
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  
}


// WINDOW UNLOAD
static void main_window_unload(Window *window) {

  // Destroy the 10 sub-images
  for (int i=0; i<10; i++){
    gbitmap_destroy(s_digit_image[i]);
  }
  
  // Destroy the source image
  gbitmap_destroy(s_digit_all_images);
  
  // Unsubscribe from the tick timer
  tick_timer_service_unsubscribe();
      
}





// INIT
static void init(void) {
    
  // Create main Window element and assign to pointer
  s_main_window = window_create();
 
  // Set the window background color to black
  window_set_background_color(s_main_window, GColorBlack);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
    
}


static void deinit(void) {
  
  // Destroy Window
  window_destroy(s_main_window);
  
}  



// MAIN PROGRAM LOOP
int main(void) {
  init();
  app_event_loop();
  deinit();
}