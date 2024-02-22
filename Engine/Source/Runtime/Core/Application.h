#pragma once

#include "AppLayer.h"
#include "Event.h"
#include "InputManager.h"
#include "Window.h"

void sk_app_init();
void sk_app_run();
void sk_app_shutdown();
void sk_app_on_event(SKEvent &e);
void sk_app_add_layer(SKAppLayer *layer);

