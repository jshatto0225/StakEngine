#pragma once

#include "Window.h"

bool platform_init();
void platform_shutdown();

Window platform_create_window(const WindowConfig *cfg);
void platform_destroy_window(Window window);

bool platform_init_imgui(Window window);

void platform_imgui_new_frame();
void platform_shutdown_imgui();
void platform_process_messages();

void platform_enable_raw_input(Window window);
void platform_disable_raw_input(Window window);
void platform_set_cursor_visibility(Window window, CursorVisibility visibility);

u32 platform_get_window_width(Window window);
u32 platform_get_window_height(Window window);

void *platform_get_window_handle(Window window);