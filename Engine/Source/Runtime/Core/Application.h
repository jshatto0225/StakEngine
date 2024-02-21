#pragma once

#include <vector>

#include "AppLayer.h"
#include "Event.h"
#include "InputManager.h"
#include "Window.h"

void sk_app_init();
void sk_app_run();
void sk_app_shutdown();
void sk_app_on_event(Event &e);
void sk_app_add_layer(AppLayer *layer);

