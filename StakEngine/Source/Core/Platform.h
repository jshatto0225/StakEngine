#pragma once

#include "Window.h"

bool platform_init();
void platform_shutdown();

bool platform_open_window(Window *window, const Window_Config *cfg);
void platform_close_window(Window *window);

bool platform_init_imgui(const Window *window);

void platform_imgui_new_frame();
void platform_shutdown_imgui();
void platform_process_messages();

void platform_enable_raw_input(const Window *window);
void platform_disable_raw_input(const Window *window);
void platform_set_cursor_visibility(const Window *window, Cursor_Visibility visibility);