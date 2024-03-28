#pragma once

#include "ApplicationLayer.h"
#include "Event.h"

struct application_spec
{
  const char *WindowTitle;
  i32 WindowX;
  i32 WindowY;
  i32 WindowWidth;
  i32 WindowHeight;
  layer_stack Layers;
};

struct application {
  application_spec Spec;
  window *Window;
  bool Running;
  layer_stack Layers;
};

application *ApplicationInit(const application_spec *Spec);
void ApplicationShutdown(application **App);
void ApplicationRun(application *App);
void ApplicationOnEvent(void *Parent, const event *Event);
void AddLayerToStack(layer_stack *Stack, layer_init Init, layer_shutdown Shutdown, layer_update Update, layer_on_event OnEvent);
