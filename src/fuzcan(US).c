/*

Fuzzy Analog watch with date

based on watchface Simple Analog by Ice2097
programming done by true rookie made possible by superb cloudpebble.net tool by Katharine
Big thank you to both
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x25, 0x2D, 0x82, 0x5C, 0xF5, 0x82, 0x40, 0xE1, 0x90, 0xBA, 0x46, 0x0B, 0x2C, 0x03, 0x50, 0x8D }
PBL_APP_INFO(MY_UUID, "Fuzzy Analog (US)", "ColdFly", 0x1, 0x0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);


Window window;

BmpContainer background_image_container;
TextLayer text_date_layer;
RotBmpContainer hour_hand_image_container;
RotBmpContainer minute_hand_image_container;



/* -------------- TODO: Remove this and use Public API ! ------------------- */

// from src/core/util/misc.h
#define MAX(a,b) (((a)>(b))?(a):(b))

//define HOUR variable to choose hour hand image
static int32_t HOUR;


//! newton's method for floor(sqrt(x)) -> should always converge

static int32_t integer_sqrt(int32_t x) {
  if (x < 0) {
    return 0;
  }

  int32_t last_res = 0;
  int32_t res = (x + 1)/2;
  while (last_res != res) {
    last_res = res;
    res = (last_res + x / last_res) / 2;
  }
  return res;
}

void rot_bitmap_set_src_ic(RotBitmapLayer *image, GPoint ic) {
  image->src_ic = ic;

  // adjust the frame so the whole image will still be visible
  const int32_t horiz = MAX(ic.x, abs(image->bitmap->bounds.size.w - ic.x));
  const int32_t vert = MAX(ic.y, abs(image->bitmap->bounds.size.h - ic.y));

  GRect r = layer_get_frame(&image->layer);
  const int32_t new_dist = (integer_sqrt(horiz*horiz + vert*vert) * 2) + 1; //// Fudge to deal with non-even dimensions--to ensure right-most and bottom-most edges aren't cut off.

  r.size.w = new_dist;
  r.size.h = new_dist;
  layer_set_frame(&image->layer, r);

  r.origin = GPoint(0, 0);
  image->layer.bounds = r;

  image->dest_ic = GPoint(new_dist / 2, new_dist / 2);

  layer_mark_dirty(&(image->layer));
}

/* ------------------------------------------------------------------------- */


void set_hand_angle(RotBmpContainer *hand_image_container, unsigned int hand_angle) {

  signed short x_fudge = 0;
  signed short y_fudge = 0;


  hand_image_container->layer.rotation =  TRIG_MAX_ANGLE * hand_angle / 360;

  // define 'fudge' to make sure hands point to 6 and 9 o'clock pointers
  if (hand_angle == 180) {
    x_fudge = -2;
  } else if (hand_angle == 270) {
    y_fudge = -1;
  }

  // (144 = screen width, 168 = screen height)
  hand_image_container->layer.layer.frame.origin.x = (144/2) - (hand_image_container->layer.layer.frame.size.w/2) + x_fudge;
  hand_image_container->layer.layer.frame.origin.y = (73) - (hand_image_container->layer.layer.frame.size.h/2) + y_fudge;

  layer_mark_dirty(&hand_image_container->layer.layer);
}


void update_hand_positions () {

  PblTm t;

  get_time(&t);

  HOUR = t.tm_hour;
  if (HOUR > 12) {HOUR = HOUR - 12;} 
	
  rotbmp_deinit_container(&hour_hand_image_container); // NOT SURE IF THIS IS REQUIRED
  
  // winded way to determine right hour hand image. 
  // I'm sure some smart cookie can program this to use t.tm_hour or HOUR variables
  if (HOUR < 1) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_0, &hour_hand_image_container);
  } 
  else if (HOUR == 1) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_1, &hour_hand_image_container);
  } 
   else if (HOUR == 2) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_2, &hour_hand_image_container);
  } 
   else if (HOUR == 3) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_3, &hour_hand_image_container);
  } 
   else if (HOUR == 4) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_4, &hour_hand_image_container);
  } 
   else if (HOUR == 5) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_5, &hour_hand_image_container);
  } 
   else if (HOUR == 6) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_6, &hour_hand_image_container);
  } 
   else if (HOUR == 7) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_7, &hour_hand_image_container);
  } 
   else if (HOUR == 8) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_8, &hour_hand_image_container);
  } 
   else if (HOUR == 9) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_9, &hour_hand_image_container);
  } 
   else if (HOUR == 10) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_10, &hour_hand_image_container);
  } 
   else if (HOUR == 11) {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_11, &hour_hand_image_container);
  } 
   else {
    rotbmp_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_12, &hour_hand_image_container);
  }
	
  rot_bitmap_set_src_ic(&hour_hand_image_container.layer, GPoint(8, 48)); // 50% of width,  max length less 50% width 

  // SET HAND ANGLES	
  set_hand_angle(&hour_hand_image_container, ((t.tm_hour % 12) * 30) + (t.tm_min/2) + 360); // Add 360 to make sure time shows at midnight
  set_hand_angle(&minute_hand_image_container, t.tm_min * 6); // OPTION - add 360 to show minute hand at full hour
  	

}





void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;
  static char date_text[] = "XXX 00 XXX";
  string_format_time(date_text, sizeof(date_text), "%b %d %a", t->tick_time);
  text_layer_set_text(&text_date_layer, date_text);
  update_hand_positions(); // TODO: Pass tick event
}



void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Fuzzy Analog");
  window_stack_push(&window, true);

  resource_init_current_app(&APP_RESOURCES);

  // Set up a layer for the static watch face background
  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image_container);
  layer_add_child(&window.layer, &background_image_container.layer.layer);

	
  // Set up a layer for the minute hand	
  rotbmp_init_container(RESOURCE_ID_IMAGE_MINUTE_HAND, &minute_hand_image_container);
  rot_bitmap_set_src_ic(&minute_hand_image_container.layer, GPoint(3, 70));
  layer_add_child(&window.layer, &minute_hand_image_container.layer.layer);

  // layer for Day and Date

  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(0, 150, 144, 22));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PIX_14)));
  text_layer_set_text_alignment(&text_date_layer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &text_date_layer.layer);	
	
  // Set up a layer for the hour hand
  // combine with set hand angles (hour hand image changes per hour)	
  
  update_hand_positions();
	
  layer_add_child(&window.layer, &hour_hand_image_container.layer.layer);
  
  
  // NOT USED - Set up a layer for the second hand

}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image_container);
  rotbmp_deinit_container(&hour_hand_image_container);
  rotbmp_deinit_container(&minute_hand_image_container);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
