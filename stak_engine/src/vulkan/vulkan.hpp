#pragma once

#include "platform/platform.hpp"

namespace Vulkan {

void init(const std::string &app_name);
void shutdown();

void *create_device(Platform::Window *win);
void destroy_device(void **);

void *create_command_list(void *device);
void destroy_command_list(void **list);

void *begin_frame(void *device, const std::vector<void *> &lists);
void end_frame(void *frame_data);

void wait_for_device(void *device);
}
