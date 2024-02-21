#include "Runtime/Runtime.h"

void test_start() {
  SK_TRACE("Source dir: %s\n", SOURCE_DIR);
}

void test_update() {

}

void test_on_event(Event &e) {

}

void extern_init() {
  AppLayer *test_layer = new AppLayer;
  test_layer->start = test_start;
  test_layer->update = test_update;
  test_layer->on_event = test_on_event;
  sk_app_add_layer(test_layer);
}
