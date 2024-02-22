#pragma once

#include "Event.h"

struct SKAppLayer {
  void (*start)() = []() {};
  void (*end)() = []() {};
  void (*update)() = []() {};
  void (*on_event)(SKEvent &e) = [](SKEvent &) {};
};

