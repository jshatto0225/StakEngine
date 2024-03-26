#pragma once

#include "Event.h"

struct application_layer {
  void *(*Init)();
  void(*Shutdown)(void **);
  void(*Update)(void *);
  void(*OnEvent)(void *, void *, const event *);
};

struct layer_stack {
  static const u32 MAX_LAYERS = 255;
  u32 Size;
  void *LayerData[MAX_LAYERS];
  application_layer Layers[MAX_LAYERS];
};
