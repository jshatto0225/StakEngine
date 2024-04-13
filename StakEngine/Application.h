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
};

void ApplicationInit(const application_spec *Spec);
void ApplicationShutdown();
void ApplicationRun();
void ApplicationOnEvent(const event *Event);
void AddLayerToStack(layer_init Init, layer_shutdown Shutdown, layer_update Update, layer_on_event OnEvent);
