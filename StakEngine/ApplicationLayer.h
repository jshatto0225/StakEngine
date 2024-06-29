#pragma once

#include "Event.h"

/**
 * @brief Typedefed functions for convenience
 * 
 */
typedef void (*layer_init)();
typedef void (*layer_shutdown)();
typedef void (*layer_update)();
typedef void (*layer_on_event)(const event *);

/**
 * @brief Struct with data for an application layer 
 */
struct application_layer
{
    layer_init Init; /**< Layer's init function */
    layer_shutdown Shutdown; /**< Layer's shutdown function */
    layer_update Update; /**< Layer's update function */
    layer_on_event OnEvent; /**< Layer's event function */
};

constexpr u32 MAX_LAYERS = 255; /**< Max number of layers in the stack */

/**
 * @brief Struct to store a list of layers
 */
struct layer_stack
{
    u32 Size; /**< Current number of layers */
    application_layer Layers[MAX_LAYERS]; /**< List of layers */
};
