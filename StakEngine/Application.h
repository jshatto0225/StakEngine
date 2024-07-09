#pragma once

#include "ApplicationLayer.h"
#include "Event.h"

namespace Application
{

/**
 * @brief Struct to tell the app how to initialize
 * 
 */
struct spec
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
void Init(const spec *Spec);

/**
 * @brief Function to shut the app down 
 */
void Shutdown();

/**
 * @brief Function containing main app loop
 */
void Run();

/**
 * @brief Function to notify the app of an event
 * 
 * @param Event Event to sent to the app
 */
void OnEvent(const event *Event);

/**
 * @brief Function to add a layer to the app
 * 
 * @param Init Layer's init function
 * @param Shutdown Layer's shutdown function
 * @param Update Layer's update function
 * @param OnEvent Layer's event function
 */
void AddLayerToStack(layer_init Init, layer_shutdown Shutdown, layer_update Update, layer_on_event OnEvent);

/**
 * @brief Function to tell the application that the main window should close after the current loop
 */
void RequestShutdown();

} // namespace Application
