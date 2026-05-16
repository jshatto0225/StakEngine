#include "RHI.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
#define rhi_impl_init vulkan_init
#define rhi_impl_shutdown vulkan_shutdown
#endif

static RHI rhi;

bool rhi_init(_nullable RHIAllocator *alloc, _nullable RHIAllocator *temp_alloc) {
    return rhi_impl_init(&rhi, alloc, temp_alloc);
}

void rhi_shutdown() {
    rhi_impl_shutdown();
}

// Memory
void *rhi_alloc(RHIDevice device, u64 bytes, RHIMemoryType memory) {
    return rhi.alloc(device, bytes, memory);
}

void rhi_free(RHIDevice device, _nullable void *ptr) {
    rhi.free(device, ptr);
}

void *rhi_host_to_device_pointer(RHIDevice device, void *ptr) {
    return rhi.host_to_device_pointer(device, ptr);
}

// Device
RHIDevice rhi_create_device(RHIDeviceDesc *desc) {
    return rhi.create_device(desc);
}

void rhi_destroy_device(RHIDevice device) {
    rhi.destroy_device(device);
}

void rhi_device_wait_idle(RHIDevice device) {
    rhi.device_wait_idle(device);
}

// Swapchain
RHISwapchain rhi_create_swapchain(RHIDevice device, RHISwapchainDesc *desc) {
    return rhi.create_swapchain(device, desc);
}

void rhi_destroy_swapchain(RHIDevice device, RHISwapchain swapchain) {
    rhi.destroy_swapchain(device, swapchain);
}

RHITexture rhi_next_backbuffer(RHISwapchain swapchain) {
    return rhi.next_backbuffer(swapchain);
}

void rhi_present(RHISwapchain swapchain, RHITexture texture) {
    rhi.present(swapchain, texture);
}

// Textures
RHITextureSizeAlign rhi_texture_size_align(RHIDevice device, RHITextureDesc *desc) {
    return rhi.texture_size_align(device, desc);
}

RHITexture rhi_create_texture(RHIDevice device, RHITextureDesc *desc, void *ptr_gpu) {
    return rhi.create_texture(device, desc, ptr_gpu);
}

void rhi_destroy_texture(RHIDevice device, RHITexture texture) {
    rhi.destroy_texture(device, texture);
}

RHITextureDescriptor rhi_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc) {
    return rhi.texture_view_descriptor(device, texture, desc);
}

RHITextureDescriptor rhi_rw_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc) {
    return rhi.rw_texture_view_descriptor(device, texture, desc);
}

// Pipelines
RHIPipeline rhi_create_compute_pipeline(RHIDevice device, u8 *compute_ir, u32 ir_size) {
    return rhi.create_compute_pipeline(device, compute_ir, ir_size);
}

RHIPipeline rhi_create_graphics_pipeline(RHIDevice device, u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    return rhi.create_graphics_pipeline(device, vertex_ir, vertex_ir_size, pixel_ir, pixel_ir_size, desc);
}

RHIPipeline rhi_create_graphics_meshlet_pipeline(RHIDevice device, u8 *meshlet_ir, u32 meshlet_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    return rhi.create_graphics_meshlet_pipeline(device, meshlet_ir, meshlet_ir_size, pixel_ir, pixel_ir_size, desc);
}

void rhi_destroy_pipeline(RHIDevice device, RHIPipeline pipeline) {
    rhi.destroy_pipeline(device, pipeline);
}

// State objects
RHIDepthStencilState rhi_create_depth_stencil_state(RHIDevice device, RHIDepthStencilDesc *desc) {
    return rhi.create_depth_stencil_state(device, desc);
}

RHIBlendState rhi_create_blend_state(RHIDevice device, RHIBlendDesc *desc) {
    return rhi.create_blend_state(device, desc);
}

void rhi_free_depth_stencil_state(RHIDevice device, RHIDepthStencilState state) {
    return rhi.free_depth_stencil_state(device, state);
}

void rhi_free_blend_state(RHIDevice device, RHIBlendState state) {
    return rhi.free_blend_state(device, state);
}

// Queue
RHIQueue rhi_get_queue(RHIDevice device, RHIQueueDesc *desc) {
    return rhi.get_queue(device, desc);
}

RHICommandBuffer rhi_start_command_recording(RHIQueue queue) {
    return rhi.start_command_recording(queue);
}

void rhi_submit(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count, RHISemaphore semaphore, u64 semaphore_value) {
    rhi.submit(queue, command_buffers, command_buffer_count, semaphore, semaphore_value);
}

// Semaphores
RHISemaphore rhi_create_semaphore(RHIDevice device, u64 init_value) {
    return rhi.create_semaphore(device, init_value);
}

void rhi_wait_semaphore(RHIDevice device, RHISemaphore semaphore, u64 value) {
    rhi.wait_semaphore(device, semaphore, value);
}

void rhi_destroy_semaphore(RHIDevice device, RHISemaphore semaphore) {
    rhi.destroy_semaphore(device, semaphore);
}

// Commands
void rhi_mem_copy(RHICommandBuffer cb, void *dest_gpu, void *src_gpu, u64 size) {
    rhi.mem_copy(cb, dest_gpu, src_gpu, size);
}

void rhi_copy_to_texture(RHICommandBuffer cb, RHITexture texture, void *src_gpu) {
    rhi.copy_to_texture(cb, texture, src_gpu);
}

void rhi_copy_from_texture(RHICommandBuffer cb, void *dest_gpu, RHITexture texture) {
    rhi.copy_from_texture(cb, dest_gpu, texture);
}

void rhi_set_active_texture_heap_ptr(RHICommandBuffer cb, void *ptr_gpu, u64 size) {
    rhi.set_active_texture_heap_ptr(cb, ptr_gpu, size);
}

void rhi_barrier(RHICommandBuffer cb, RHIPipelineStage before, RHIPipelineStage after, RHIHazardFlags hazards) {
    rhi.barrier(cb, before, after, hazards);
}

void rhi_signal_after(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value) {
    rhi.signal_after(cb, after, sem, value);
}

void rhi_wait_before(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value) {
    rhi.wait_before(cb, after, sem, value);
}

void rhi_set_pipeline(RHICommandBuffer cb, RHIPipeline pipeline) {
    rhi.set_pipeline(cb, pipeline);
}

void rhi_set_depth_stencil_state(RHICommandBuffer cb, RHIDepthStencilState state) {
    rhi.set_depth_stencil_state(cb, state);
}

void rhi_set_blend_state(RHICommandBuffer cb, RHIBlendState state) {
    rhi.set_blend_state(cb, state);
}

void rhi_dispatch(RHICommandBuffer cb, void *data_gpu, u32 grid_dimensions[3]) {
    rhi.dispatch(cb, data_gpu, grid_dimensions);
}

void rhi_dispatch_indirect(RHICommandBuffer cb, void *data_gpu, void *grid_dimensions_gpu) {
    rhi.dispatch_indirect(cb, data_gpu, grid_dimensions_gpu);
}

void rhi_begin_render_pass(RHICommandBuffer cb, RHIRenderPassDesc *desc) {
    rhi.begin_render_pass(cb, desc);
}

void rhi_end_render_pass(RHICommandBuffer cb) {
    rhi.end_render_pass(cb);
}

void rhi_draw_indexed_instanced(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count) {
    rhi.draw_indexed_instanced(cb, vertex_data_gpu, pixel_data_gpu, indices_gpu, index_count, instance_count);
}

void rhi_draw_indexed_instanced_indirect(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, void *args_gpu) {
    rhi.draw_indexed_instanced_indirect(cb, vertex_data_gpu, pixel_data_gpu, indices_gpu, args_gpu);
}

void rhi_draw_indexed_instanced_indirect_multi(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *args_gpu, void *draw_count_gpu, u32 stride) {
    rhi.draw_indexed_instanced_indirect_multi(cb, vertex_data_gpu, pixel_data_gpu, args_gpu, draw_count_gpu, stride);
}

void rhi_draw_meshlets(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, u32 dim[3]) {
    rhi.draw_meshlets(cb, meshlet_data_gpu, pixel_data_gpu, dim);
}

void rhi_draw_meshlets_indirect(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, void *dim_gpu) {
    rhi.draw_meshlets_indirect(cb, meshlet_data_gpu, pixel_data_gpu, dim_gpu);
}