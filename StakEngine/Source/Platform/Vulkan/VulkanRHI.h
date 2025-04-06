#pragma once

#include "Types.h"
#include "RHI.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

struct FVulkanViewport;

#define MAX_FRAMES_IN_FLIGHT 2

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef SK_DEBUG
const bool ENABLE_VALIDATION_LAYERS = true;
#else
const bool ENABLE_VALIDATION_LAYERS = false;
#endif

struct Vulkan_Swapchain_Support {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

enum class Vulkan_Queue {
    GRAPHICS,
    PRESENT,
    ANY,
};

struct Vulkan_Texture {
    bool swapchain_texture;
    VkImage image;
    VkDeviceMemory image_memory;
    VkImageView image_view;
    VkSampler sampler;
    VkImageSubresourceRange *subresource_ranges;
    u32 subresource_range_count;
    VkFormat format;
    VkExtent2D extent;
    u32 image_count;
    VkDescriptorSet imgui_descriptor_set;
    Rhi_Render_Area render_area;

    bool is_shader_resource;
    bool is_imgui_compatible;
};

struct Vulkan_Swapchain {
    u32 image_count;
    u32 min_image_count;
    bool framebuffer_resized;
    Vulkan_Texture *images;
    u32 image_index;
    VkFormat format;
    VkSemaphore image_available_semaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore render_finished_semaphores[MAX_FRAMES_IN_FLIGHT];
    VkQueue present_queue;
    u32 present_queue_index;
    VkSwapchainKHR swapchain;
    Handle window_handle;
    VkSurfaceKHR surface;
};

struct Vulkan_Command_List {
    VkCommandBuffer buffers[MAX_FRAMES_IN_FLIGHT];
};

struct Vulkan_Shader {
    VkShaderStageFlagBits state;
    VkShaderModule shader;
};

struct Vulkan_Buffer {
    Rhi_Buffer_Type type;
    u32 size;
    u32 element_count;
    Rhi_Buffer_Layout layout;
    bool use_staging_buffer;
    VkBuffer buffer;
    VkBuffer staging_buffer;
    VkDeviceMemory buffer_memory;
    VkDeviceMemory staging_buffer_memory;
    void *mapped_data;
};

struct Vulkan_Pipeline_Layout {
    VkPipelineLayout layout;
};

struct Vulkan_Descriptor_Set_Layout {
    VkDescriptorSetLayout layout;
};

struct Vulkan_Pipeline {
    VkPipelineBindPoint bind_point;
    VkPipeline pipeline;
};

bool vulkan_get_swapchain_support(Vulkan_Swapchain_Support *out, VkSurfaceKHR surface);
u32 vulkan_find_memory_type(u32 filter, VkMemoryPropertyFlags flags);
VkAccessFlags vulkan_get_access_mask(Rhi_Resource_State state);
VkImageLayout vulkan_get_image_layout(Rhi_Resource_State state);
Vulkan_Queue vulkan_get_queue(Rhi_Resource_State state);
VkPipelineStageFlags vulkan_get_pipeline_stage_mask(Rhi_Resource_State state);
VkIndexType vulkan_get_index_type(Rhi_Format format);
VkDescriptorType vulkan_get_descriptor_type(Rhi_Descriptor_Type type);
VkShaderStageFlags vulkan_get_shader_stage_flags(std::vector<Rhi_Shader_Type> types);
VkShaderStageFlagBits vulkan_get_shader_stage(Rhi_Shader_Type type);
VkVertexInputRate vulkan_get_vertex_input_rate(Rhi_Vertex_Input_Rate input_rate);
VkFormat vulkan_get_format(Rhi_Format format);
VkFormat vulkan_get_depth_format(Rhi_Format format);
Rhi_Format vulkan_get_rhi_format(VkFormat format);
VkFormat vulkan_get_stencil_format(Rhi_Format format);
s32 find_present_queue_index(VkSurfaceKHR surface);
u32 vulkan_find_memory_type(u32 filter, VkMemoryPropertyFlags flags);
VkCommandBuffer vulkan_begin_one_time_command_buffer();
bool vulkan_end_one_time_command_buffer(VkCommandBuffer buffer);
bool vulkan_copy_buffer(VkBuffer dst, VkBuffer src, u32 size);
bool vulkan_create_buffer(u32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory);
bool vulkan_recreate_swapchain(Vulkan_Swapchain *swapchain);
bool vulkan_create_swapchain_texture(Vulkan_Texture *out, VkImage image, VkExtent2D extent, VkFormat format);
bool vulkan_create_image_view(VkImageView *out, VkImage image, VkFormat format);
bool vulkan_create_image(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags flags, VkMemoryPropertyFlags properties, VkImage *out_image, VkDeviceMemory *out_image_memory);
bool vulkan_create_texture_sampler(VkSampler *out);
void vulkan_transition_barrier(Vulkan_Command_List *list, Rhi_Transition_Barrier *barrier);

// RHI:
Rhi vulkan_create_rhi();

bool vulkan_init();
void vulkan_shutdown();

void vulkan_imgui_new_frame();
void vulkan_shutdown_imgui();
void vulkan_init_imgui(Rhi_Resource_Handle swapchain);

bool vulkan_wait_for_gpu_idle();

bool vulkan_prepare_frame(Rhi_Resource_Handle swapchain);
bool vulkan_present_frame();
bool vulkan_submit(Rhi_Resource_Handle command_list);

void vulkan_add_texture_to_imgui_window(Rhi_Resource_Handle texture);

Rhi_Resource_Handle vulkan_create_texture(Rhi_Texture_Description *description);
void vulkan_destroy_texture(Rhi_Resource_Handle *resource);

Rhi_Resource_Handle vulkan_create_swapchain(Window *window);
void vulkan_destroy_swapchain(Rhi_Resource_Handle *resource);

Rhi_Resource_Handle vulkan_create_command_list(Rhi_Command_List_Description *description);
void vulkan_destroy_command_list(Rhi_Resource_Handle *resource);

Rhi_Resource_Handle vulkan_create_shader(Rhi_Shader_Description *description);
void vulkan_destroy_shader(Rhi_Resource_Handle *resource);

Rhi_Resource_Handle vulkan_create_vertex_buffer(Rhi_Vertex_Buffer_Description *description);
Rhi_Resource_Handle vulkan_create_index_buffer(Rhi_Index_Buffer_Description *description);
void vulkan_destroy_buffer(Rhi_Resource_Handle *resource);

Rhi_Resource_Handle VulkanCreatePipelineLayout(Rhi_Pipeline_Layout_Description *description);
void VulkanDestroyPipelineLayout(Rhi_Resource_Handle *resource);

Rhi_Resource_Handle vulkan_create_descriptor_set_layout(Rhi_Descriptor_Set_Layout_Description *description);
void vulkan_destroy_descriptor_set_layout(Rhi_Resource_Handle *resource);

Rhi_Resource_Handle vulkan_create_graphics_pipeline(Rhi_Graphics_Pipeline_State_Description *description);
void vulkan_destroy_pipeline(Rhi_Resource_Handle *resource);

void vulkan_set_buffer_data(Rhi_Resource_Handle buffer, void *data, u32 size);
void vulkan_map_buffer_data(Rhi_Resource_Handle buffer, void **data, u32 size);
bool vulkan_send_buffer_to_gpu(Rhi_Resource_Handle buffer);

Rhi_Resource_Handle vulkan_get_current_swapchain_texture(Rhi_Resource_Handle swapchain);
Rhi_Format vulkan_get_texture_format(Rhi_Resource_Handle texture);
Rhi_Render_Area vulkan_get_texture_render_area(Rhi_Resource_Handle texture);

bool vulkan_begin_command_list(Rhi_Resource_Handle command_list);
bool vulkan_end_command_list(Rhi_Resource_Handle command_list);

void vulkan_cmd_render_imgui_draw_data(Rhi_Resource_Handle command_list, ImDrawData *draw_data);
void vulkan_cmd_resource_barrier(Rhi_Resource_Handle command_list, Rhi_Resource_Barrier *barrier);
void vulkan_cmd_set_render_target(Rhi_Resource_Handle command_list, Rhi_Resource_Handle target, Rhi_Render_Area *render_area);
void vulkan_cmd_unset_render_target(Rhi_Resource_Handle command_list);
void vulkan_cmd_bind_vertex_buffer(Rhi_Resource_Handle command_list, Rhi_Resource_Handle buffer, u32 first_vertex);
void vulkan_cmd_bind_index_buffer(Rhi_Resource_Handle command_list, Rhi_Resource_Handle buffer);
void vulkan_cmd_draw_indexed(Rhi_Resource_Handle command_list, u32 index_count, u32 instance_count, u32 first_index, s32 vertex_offset, u32 first_instance);
void vulkan_cmd_draw_instanced(Rhi_Resource_Handle command_list, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance);
void vulkan_cmd_set_viewport(Rhi_Resource_Handle command_list, f32 x, f32 y, f32 width, f32 height, f32 min_depth, f32 max_depth);
void vulkan_cmd_set_scissor(Rhi_Resource_Handle command_list, s32 x, s32 y, u32 width, u32 height);
void vulkan_cmd_bind_pipeline(Rhi_Resource_Handle command_list, Rhi_Resource_Handle pipeline);

void vulkan_notify_swapchain_of_resize(Rhi_Resource_Handle swapchain);