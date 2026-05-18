#include "SK.h"

#ifdef SK_VULKAN
#include "VulkanSK.h"
#define rhi_impl_load   vulkan_load
#define rhi_impl_unload vulkan_unload
#endif


PFN_sk_init     sk_init;
PFN_sk_shutdown sk_shutdown;

PFN_sk_alloc              sk_alloc;
PFN_sk_free               sk_free;
PFN_sk_host_to_device_ptr sk_host_to_device_ptr;

PFN_sk_create_device    sk_create_device;
PFN_sk_destroy_device   sk_destroy_device;
PFN_sk_device_wait_idle sk_device_wait_idle;

PFN_sk_create_swapchain  sk_create_swapchain;
PFN_sk_destroy_swapchain sk_destroy_swapchain;
PFN_sk_next_backbuffer   sk_next_backbuffer;
PFN_sk_present           sk_present;

PFN_sk_texture_size_align         sk_texture_size_align;
PFN_sk_create_texture             sk_create_texture;
PFN_sk_destroy_texture            sk_destroy_texture;
PFN_sk_texture_view_descriptor    sk_texture_view_descriptor;
PFN_sk_rw_texture_view_descriptor sk_rw_texture_view_descriptor;

PFN_sk_create_compute_pipeline          sk_create_compute_pipeline;
PFN_sk_create_graphics_pipeline         sk_create_graphics_pipeline;
PFN_sk_create_graphics_meshlet_pipeline sk_create_graphics_meshlet_pipeline;
PFN_sk_destroy_pipeline                 sk_destroy_pipeline;

PFN_sk_create_depth_stencil_state sk_create_depth_stencil_state;
PFN_sk_create_blend_state         sk_create_blend_state;
PFN_sk_free_depth_stencil_state   sk_free_depth_stencil_state;
PFN_sk_free_blend_state           sk_free_blend_state;

PFN_sk_get_queue               sk_get_queue;
PFN_sk_start_command_recording sk_start_command_recording;
PFN_sk_submit                  sk_submit;

PFN_sk_create_semaphore  sk_create_semaphore;
PFN_sk_wait_semaphore    sk_wait_semaphore;
PFN_sk_destroy_semaphore sk_destroy_semaphore;

PFN_sk_mem_copy          sk_mem_copy;
PFN_sk_copy_to_texture   sk_copy_to_texture;
PFN_sk_copy_from_texture sk_copy_from_texture;

PFN_sk_set_active_texture_heap_ptr sk_set_active_texture_heap_ptr;

PFN_sk_barrier      sk_barrier;
PFN_sk_signal_after sk_signal_after;
PFN_sk_wait_before  sk_wait_before;

PFN_sk_signal_after2 sk_signal_after2;
PFN_sk_wait_before2  sk_wait_before2;

PFN_sk_set_pipeline            sk_set_pipeline;
PFN_sk_set_depth_stencil_state sk_set_depth_stencil_state;
PFN_sk_set_blend_state         sk_set_blend_state;

PFN_sk_dispatch          sk_dispatch;
PFN_sk_dispatch_indirect sk_dispatch_indirect;

PFN_sk_begin_render_pass sk_begin_render_pass;
PFN_sk_end_render_pass   sk_end_render_pass;

PFN_sk_draw_indexed_instanced                sk_draw_indexed_instanced;
PFN_sk_draw_indexed_instanced_indirect       sk_draw_indexed_instanced_indirect;
PFN_sk_draw_indexed_instanced_indirect_multi sk_draw_indexed_instanced_indirect_multi;

PFN_sk_draw_meshlets          sk_draw_meshlets;
PFN_sk_draw_meshlets_indirect sk_draw_meshlets_indirect;

void sk_load() {
    rhi_impl_load();
}

void sk_unload() {
    rhi_impl_unload();
}
