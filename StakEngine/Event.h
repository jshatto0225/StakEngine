#pragma once

#include "Types.h"

struct window;

/**
 * @brief Event types enum
 */
enum event_type
{
    TEST_EVENT = -1, /**< Event for testing */
    INVALID_EVENT = -1, /**< Invalid event */
    WINDOW_RESIZED, /**< Window resize event */
    WINDOW_MOVED, /**< Window move event */
    WINDOW_CLOSE, /**< Windows close event */
    WINDOW_CREATE, /**< Window created event */
    WINDOW_PAINT, /**< Window draw event */
};

/**
 * @brief Data for a window paint event
 */
struct window_paint_event
{
    window *Window; /**< Window tied to the event */
};

/**
 * @brief Data for a window close event
 */
struct window_close_event
{
    const window *Window; /**< Window linked to event */
};

/**
 * @brief Data for a window move event
 */
struct window_move_event
{
    const window *Window; /**< Window linked to event */
    i32 X; /**< New x pos */
    i32 Y; /**< New y pos */
};

/**
 * @brief Data for window resize event
 */
struct window_resize_event
{
    const window *Window; /**< Window linked to event */
    i32 Width; /**< New width */
    i32 Height; /**< New height */
};

/**
 * @brief Struct for events containing type and associated data 
 */
struct event
{
    event_type Type; /**< Type of event */
    window_close_event WinCloseEvent; /**< Window close event data */
    window_move_event WinMoveEvent; /**< Window move event data */
    window_resize_event WinResizeEvent; /**< Window resize event data */
    window_paint_event WinPaintEvent; /**< Window paint event data */
};

/**
 * @brief Type for all event functions
 */
typedef void(*EventFn)(const event *);
