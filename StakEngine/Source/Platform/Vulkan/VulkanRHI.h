#pragma once

#include "RHI.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

class Vulkan_Rhi_Command_Context;

class Vulkan_Rhi final : public Rhi {
public:
    Vulkan_Rhi();
    ~Vulkan_Rhi() override;

    Rhi_Sampler_State_Ref create_sampler_state(const Rhi_Sampler_State_Initializer &initializer) override;
    Rhi_Rasterizer_State_ref create_rasterizer_state(const Rhi_Rasterizer_State_Initializer &initializer) override;
    Rhi_Depth_Stencil_State_Ref create_depth_stencil_state(const Rhi_Depth_Stencil_State_Initializer &initializer) override;
    Rhi_Blend_State_Ref create_blend_state(const Rhi_Blend_State_Initializer &initializer) override;
    Rhi_Vertex_Declaration_Ref create_vertex_declaration(const Rhi_Vertex_Declaration_Element_List &elements) override;

    Rhi_Pixel_Shader_Ref create_pixel_shader() override;
    Rhi_Vertex_Shader_Ref create_vertex_shader() override;
    Rhi_Geometry_Shader_Ref create_geometry_shader() override;

    Rhi_Buffer_Ref create_buffer(const Rhi_Buffer_Description &description, Rhi_Access access) override;
    Rhi_Uniform_Buffer_Ref create_uniform_buffer(const void *initial_contents, const Rhi_Uniform_Buffer_Layout &layout, Rhi_Uniform_Buffer_Usage usage) override;
    Rhi_Staging_Buffer_Ref create_staging_buffer() override;

    Rhi_Texture_Ref create_texture(const Rhi_Texture_Description &description) override;

    Rhi_Graphics_Pipeline_State_Ref create_graphics_pipeline_state(const Rhi_Graphics_Pipeline_State_Initializer &initializer) override;

    Rhi_Shader_Resource_View_Ref create_shader_resource_view(Rhi_Viewable_Resource_Ref resource, const Rhi_View_Description &description) override;
    Rhi_Unordered_Access_View_Ref create_unordered_access_view(Rhi_Viewable_Resource_Ref resource, const Rhi_View_Description &description) override;

    void init_imgui() override;
    void shutdown_imgui() override;
    void imgui_new_frame() override;
    void submit_imgui_draw_data(ImDrawData *draw_data) override;
  
    inline void is_framebuffer_resized() override { framebuffer_resized = true; }

    void begin_drawing() override;
    void end_drawing() override;

    void wait_for_gpu_idle() override;
  
    Rhi_Command_Context *get_command_context() override;
  
public:
    inline VkSwapchainKHR get_vk_swapchain() const { return swapchain; }
    inline VkSemaphore get_vk_render_finished_semaphore() const { return render_finished_semaphores[current_frame]; }
    inline VkQueue get_vk_present_queue() const { return present_queue; }
    
private:
    struct Queue_Families {
        std::optional<u32> graphics_family;
        std::optional<u32> present_family;
    };
    
    Queue_Families find_queue_families(VkPhysicalDevice device);
    
    struct Swapchain_Support {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    Swapchain_Support get_swapchain_support(VkPhysicalDevice device);

    void create_swapchain();
    void recreate_swapchain();
    void create_image_views();
    VkImageView create_image_view(VkImage image, VkFormat format);
    void create_sync_objects();
  
private:
    Vulkan_Rhi_Command_Context *command_context;

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    u32 present_queue_index = UINT32_MAX;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    u32 graphics_queue_index = UINT32_MAX;
    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkImage> swapchain_images;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkExtent2D extent = {};
    VkSurfaceFormatKHR swapchain_image_format = {};
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImageView> swapchain_image_views;
    std::vector<VkSemaphore> render_finished_semaphores;
    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkFence> in_flight_fences;
    VkDescriptorPool imgui_pool = VK_NULL_HANDLE;
    VkCommandPool command_pool = VK_NULL_HANDLE;
    u32 current_frame = 0;
    // TODO
    u32 image_index = 0;
    bool framebuffer_resized = false;

private:
    const std::vector<const char *> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };
    
    const std::vector<const char *> DEVICE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    const u32 MAX_FRAMES_IN_FLIGHT = 2;
    
#ifdef SK_DEBUG
    const bool ENABLE_VALIDATION_LAYERS = true;
#else
    const bool ENABLE_VALIDATION_LAYERS = false;
#endif
};
