#pragma once

#include <string>

#include "utils/types.hpp"
#include "events/events.hpp"

namespace Platform {

struct Window_Config {
	std::string title = "";

	s32 width = 0;
	s32 height = 0;
	s32 x = 0;
	s32 y = 0;
};

struct Window_Callbacks {
	void (*close)() = nullptr;
	void (*resize)(Window_Resize_Event *) = nullptr;
	void (*key)(Key_Event *) = nullptr;
	void (*mouse_button)(Mouse_Button_Event *) = nullptr;
	void (*mouse_move)(Mouse_Move_Event *) = nullptr;
};

struct Window;

Window *create_window(Window_Config *config, Window_Callbacks *callbacks);
void destroy_window(Window **window);

void poll_events();

void enable_raw_input(Window *window);
void disable_raw_input(Window *window);

void hide_cursor(Window *window);
void show_cursor(Window *window);

void get_window_size(Window *window, s32 *width, s32 *height);

}
