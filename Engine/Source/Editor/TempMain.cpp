#include "Runtime/Runtime.h"

void test_start() {
  sk_debug_trace("Source dir: %s\n", SOURCE_DIR);
}

void test_update() {
  
}

void test_on_event(SKEvent &e) {
  
}

void extern_init() {
  SKAppLayer *test_layer = new SKAppLayer;
  test_layer->start = test_start;
  test_layer->update = test_update;
  test_layer->on_event = test_on_event;
  sk_app_add_layer(test_layer);
}
