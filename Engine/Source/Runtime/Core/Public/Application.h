#pragma once

#include <vector>

#include "ApplicationLayer.h"
#include "Event.h"
#include "InputManager.h"
#include "Window.h"

namespace sk {
void application_init();
void application_run();
void application_shutdown();

void application_on_event(Event &e);

void application_add_layer(Unique<ApplicationLayer> layer);
} // namespace SK
