#include <pebble.h>
#include <pebble_fonts.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *runningText;
static TextLayer *bpmlabel;

char* reading;
bool running;
static int bpm;
static float mspb;
static GFont custom_font_72;

char *itoa(int num) {
  static char buff[20] = {};
  int i = 0, temp_num = num, length = 0;
  char *string = buff;
  if(num >= 0) {
    while(temp_num) {
      temp_num /= 10;
      length++;
    }
    for(i = 0; i < length; i++) {
    buff[(length-1)-i] = '0' + (num % 10);
    num /= 10;
    }
    buff[i] = '\0';
  } else return "Unsupported Number";
    return string;
}

static uint32_t min(int x, int y) {
  if (x > y){
    return y;
  } return x;
}


static void beat(void){
  if (running) {
    uint32_t segments[] = {min(150, 150 * 80/bpm)};
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
    };
    vibes_enqueue_custom_pattern(pat);
    app_timer_register(mspb-100, (void*)beat, NULL);
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  running = 1 - running;
  if (running) {
    text_layer_set_text(runningText, "ON");
    beat();
  } else {
    text_layer_set_text(runningText, "OFF");
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  bpm += 2;
  mspb = 1/((float)bpm/60000);
  reading = itoa(bpm);
  text_layer_set_text(text_layer, reading);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  bpm -= 2;
  mspb = 1/((float)bpm/60000);
  reading = itoa(bpm);
  text_layer_set_text(text_layer, reading);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  text_layer = text_layer_create(GRect(0, 10, 144, 78));
  text_layer_set_text(text_layer, reading);
  custom_font_72 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HELVETICA_LIGHT_72));
  text_layer_set_font(text_layer, custom_font_72);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  bpmlabel = text_layer_create(GRect(0, 86, 144, 20));
  text_layer_set_text(bpmlabel, "bpm");
  text_layer_set_font(bpmlabel, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(bpmlabel, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(bpmlabel));
  
  runningText = text_layer_create(GRect(0, 108, 144, 50));
  text_layer_set_text(runningText, "OFF");
  text_layer_set_font(runningText, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(runningText, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(runningText));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  bpm = 80;
  reading = itoa(80);
  mspb = 1/((float)bpm/60000);
  running = 0;
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
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}