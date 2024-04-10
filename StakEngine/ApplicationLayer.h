#pragma once

#include "Event.h"

typedef void *(*layer_init)();
typedef void (*layer_shutdown)(void **);
typedef void (*layer_update)(void *);
typedef void (*layer_on_event)(void *, const event *);

struct application_layer
{
    layer_init Init;
    layer_shutdown Shutdown;
    layer_update Update;
    layer_on_event OnEvent;
};

constexpr u32 MAX_LAYERS = 255;

struct layer_stack
{
    u32 Size;
    void *LayerData[MAX_LAYERS];
    application_layer Layers[MAX_LAYERS];
};
