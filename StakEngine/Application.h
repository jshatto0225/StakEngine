#pragma once

#include "ApplicationLayer.h"
#include "Event.h"

/**
 * @brief Struct to tell the app how to initialize
 * 
 */
struct application_spec
{
    const char *WindowTitle; /**< Title of app's main window */
    i32 WindowX; /**< Initial x position of main window */
    i32 WindowY; /**< Initial y position of main window */
    i32 WindowWidth; /**< Initial width of window */
    i32 WindowHeight; /**< Initial height of window */
};

/**
 * @brief Function to initialize the application 
 * 
 * @param Spec Specification to initialize the application with
 */
void ApplicationInit(const application_spec *Spec);

/**
 * @brief Function to shut the app down 
 */
void ApplicationShutdown();

/**
 * @brief Function containing main app loop
 */
void ApplicationRun();

/**
 * @brief Function to notify the app of an event
 * 
 * @param Event Event to sent to the app
 */
void ApplicationOnEvent(const event *Event);

/**
 * @brief Function to add a layer to the app
 * 
 * @param Init Layer's init function
 * @param Shutdown Layer's shutdown function
 * @param Update Layer's update function
 * @param OnEvent Layer's event function
 */
void AddLayerToStack(layer_init Init, layer_shutdown Shutdown, layer_update Update, layer_on_event OnEvent);
