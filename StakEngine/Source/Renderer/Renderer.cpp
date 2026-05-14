#include "Renderer.h"

#include "Window.h"
#include "RHI.h"
#include "Platform.h"
#include "Log.h"

bool renderer_init(Renderer *renderer, Window *Win, bool render_to_offscreen_buffer) {
    renderer->window = Win;
    renderer->use_offscreen_buffer = render_to_offscreen_buffer;

    renderer->device = rhi.create_device();
    if (!renderer->device) {
        SK_LOG_ERROR("Failed to create rhi device");
        return false;
    }

    renderer->queue = rhi.create_queue(renderer->device);
    if (!renderer->queue) {
        SK_LOG_ERROR("Failed to initialize pipeline");
        return false;
    }

    std::vector<u8> vertex_ir;
    std::vector<u8> pixel_ir;
    RHIRasterDesc raster_description = {};
    renderer->pipeline = rhi.create_graphics_pipeline(renderer->device, vertex_ir.data(), vertex_ir.size(), pixel_ir.data(), pixel_ir.size(), &raster_description);
    if (!renderer->pipeline) {
        SK_LOG_ERROR("Failed to initialize pipeline");
        return false;
    }

    renderer->next_frame = 1;

    renderer->semaphore = rhi.create_semaphore(renderer->device, 0);

    return true;
}

void renderer_shutdown(Renderer *renderer) {
    rhi.device_wait_idle(renderer->device);
    rhi.destroy_pipeline(renderer->device, renderer->pipeline);
}

bool renderer_render(Renderer *renderer) {
    if (renderer->next_frame > renderer->max_frames_in_flight) {
        rhi.wait_semaphore(renderer->device, renderer->semaphore, renderer->next_frame - renderer->max_frames_in_flight);
    }

    RHICommandBuffer cb = rhi.start_command_recording(renderer->queue);
    {
        RHIRenderPassDesc rp = {};
        rhi.begin_render_pass(cb, &rp);
        {
            rhi.set_pipeline(cb, renderer->pipeline);
            rhi.draw_indexed_instanced(cb, nullptr, nullptr, nullptr, 0, 0);
        }
        rhi.end_render_pass(cb);
    }
    rhi.submit(renderer->queue, &cb, 1, renderer->semaphore, renderer->next_frame++);

    return true;
}
