#pragma once

#include "Event.h"

struct AppLayer {
  void (*start)() = []() {};
  void (*end)() = []() {};
  void (*update)() = []() {};
  void (*on_event)(Event &e) = [](Event &) {};
};

