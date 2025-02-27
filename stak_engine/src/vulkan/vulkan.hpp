#pragma once

#include <vulkan/vulkan.h>

#include "rhi.hpp"
#include <imgui.h>

struct Vulkan_Command_List;
struct Vulkan_Device;
struct Vulkan_Sync_Object;
struct Vulkan_Render_Pass;
struct Vulkan_Render_Target;

#define MAX_FRAMES_IN_FLIGHT 2

struct Vulkan_Device : public Rhi_Device {
    Vulkan_Device(Window *window);
    ~Vulkan_Device() override;
    
    Ref<Rhi_Command_List> create_command_list() override;
    Ref<Rhi_Sync_Object> create_sync_object(Rhi_Pipeline_Stage_Mask wait_stages);
    void wait_for_idle() override;
    void new_frame() override;
    void present() override;
    void submit(const Array<Ref<Rhi_Command_List>> &lists, const Array<Ref<Rhi_Sync_Object>> &waits, const Array<Ref<Rhi_Sync_Object>> &signals) override;
    void manual_shutdown() override;
    Ref<Rhi_Render_Pass> create_render_pass(const Rhi_Render_Pass_Description &desc) override;
    Ref<Rhi_Render_Target> create_swapchain_target(Ref<Rhi_Render_Pass> pass) override;
    Rhi_Format get_swapchain_image_format() override;
    void notify_window_resize() override;
    void get_extent(s32 *width, s32 *height) override;

    inline bool is_active() override { return initialized; }

    VkImageView create_image_view(VkImage image, VkFormat format);
    void create_swapchain();
    void recreate_swapchain();

    bool initialized = false;
    Window *window = nullptr;
    
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    u32 graphics_queue_index = UINT32_MAX;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    u32 present_queue_index = UINT32_MAX;
    VkQueue present_queue = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkExtent2D extent = {};
    
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkSurfaceFormatKHR swapchain_format = {};
    Array<VkImage> swapchain_images = {};
    Array<VkImageView> swapchain_image_views = {};

    VkCommandPool command_pool = VK_NULL_HANDLE;

    Array<VkSemaphore> render_finished_semaphores = {};
    Array<VkSemaphore> image_available_semaphores = {};
    Array<VkFence> in_flight_fences = {};

    u32 current_frame = 0;
    u32 image_index = 0;
    bool framebuffer_resized = false;

    // Store created lists for deletion later, if needed
    Set<Vulkan_Command_List *> lists = {};
    Set<Vulkan_Sync_Object *> syncs = {};
    Set<Vulkan_Render_Pass *> passes = {};
    Set<Vulkan_Render_Target *> targets = {};
    Set<Vulkan_Render_Target *> backbuffers = {};
};

struct Vulkan_Command_List : Rhi_Command_List {
    Vulkan_Command_List(Vulkan_Device *d);
    ~Vulkan_Command_List() override;

    void begin() override;
    void end() override;

    void begin_render_pass(Ref<Rhi_Render_Pass> render_pass, Rhi_Subpass_Contents contents, const Rhi_Render_Pass_Info &info);
    void next_subpass(Rhi_Subpass_Contents contents);
    void end_render_pass();
    void render_imgui_draw_data(ImDrawData *data) override;
    
    void manual_shutdown();

    bool initialized = false;
    
    Vulkan_Device *device = nullptr;

    Ref<Vulkan_Render_Pass> active_render_pass = nullptr;
    u32 subpass_index = 0;
    
    VkCommandBuffer command_buffers[MAX_FRAMES_IN_FLIGHT] = {};
};

struct Vulkan_Sync_Object : Rhi_Sync_Object {
    Vulkan_Sync_Object(Vulkan_Device *device, Rhi_Pipeline_Stage_Mask wait_stages);
    ~Vulkan_Sync_Object() override;

    void manual_shutdown();

    bool initialized = false;
    
    Vulkan_Device *device = nullptr;

    Rhi_Pipeline_Stage_Mask stages = 0;

    VkSemaphore semaphores[MAX_FRAMES_IN_FLIGHT] = {};
};

struct Vulkan_Render_Pass : Rhi_Render_Pass {
    Vulkan_Render_Pass(Vulkan_Device *device, const Rhi_Render_Pass_Description &desc);
    ~Vulkan_Render_Pass();

    void manual_shutdown();

    Vulkan_Device *device = nullptr;

    bool initialized = false;

    u32 subpass_count = 0;
    VkRenderPass render_pass = VK_NULL_HANDLE;
};

struct Vulkan_Render_Target : Rhi_Render_Target {
    Vulkan_Render_Target(Vulkan_Device *device, const Rhi_Render_Target_Description &desc, Ref<Rhi_Render_Pass> render_pass);
    ~Vulkan_Render_Target() override;

    void manual_shutdown();
    void create_swapchain_framebuffers();
    void destroy_framebuffers();
    
    bool initialized = false;

    Vulkan_Device *device = nullptr;

    bool is_swapchain_backbuffer = false;

    Ref<Vulkan_Render_Pass> active_pass = {};

    Array<VkFramebuffer> framebuffers = {};

    Array<VkImage> images = {};
    Array<VkImageView> image_views = {};
};
