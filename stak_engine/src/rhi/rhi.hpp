#pragma once

#include "platform/platform.hpp"

namespace Rhi {

struct Device;
struct Render_Pass;
struct Subpass;
struct Pipeline;
struct Index_Buffer;
struct Vertex_Buffer;
struct Uniform_Buffer;
struct Command_List;
struct Shader;
struct Texture;

enum class Vendor {
	NONE,
	VULKAN,
};

void init(Vendor vendor, const std::string &app_name);
void shutdown();

Device *create_device(Platform::Window *window);
void destroy_device(Device **device);

Command_List *create_command_list(Device *device);
void destroy_command_list(Command_List **list);

}
