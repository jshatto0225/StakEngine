#pragma once

#include <string>

#include "utils.hpp"
#include "events.hpp"

void poll_events();
Array<const char *> get_vulkan_extensions();
