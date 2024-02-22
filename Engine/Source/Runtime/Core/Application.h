#pragma once

#include <vector>

#include "AppLayer.h"
#include "Event.h"
#include "InputManager.h"
#include "Window.h"

struct _SKApplication {
  bool running = false;
  std::vector<SKAppLayer *> layers;
  //InputManager *input_manager;
  SKWindow *window= nullptr;
};

void sk_app_init();
void sk_app_run();
void sk_app_shutdown();
void sk_app_on_event(SKEvent &e);
void sk_app_add_layer(SKAppLayer *layer);

