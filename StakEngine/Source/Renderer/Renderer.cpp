#include "Renderer.h"

#include "Window.h"
#include "Platform.h"
#include "RHI.h"
#include "Log.h"

Renderer *create_renderer(Window win) {
    auto renderer = (Renderer *) malloc(sizeof(Renderer));

    if (!renderer) {
        SK_LOG_ERROR("create_renderer malloc failed");
        return nullptr;
    }

    renderer->window = win;

    RHIQueueRequest queue_request = {
        .capabilities = RHI_QUEUE_GRAPHICS,
        .count = 1
    };
    RHIDeviceDesc device_desc = {
        .queues = &queue_request,
        .queue_count = 1
    };
    renderer->device = rhi.create_device(&device_desc);
    if (!renderer->device) {
        SK_LOG_ERROR("create_renderer Failed to create rhi device");
        free(renderer);
        return nullptr;
    }

    RHISwapchainDesc swapchain_desc = {
        .platform = RHI_PLATFORM_WIN32,
        .width = platform_get_window_width(win),
        .height = platform_get_window_height(win),
        .format = RHI_FORMAT_RGBA8_SRGB,
        .image_count = Renderer::max_frames_in_flight,
        .vsync = true,
        .window = platform_get_window_handle(win),
    };
    renderer->swapchain = rhi.create_swapchain(renderer->device, &swapchain_desc);
    if (!renderer->swapchain) {
        SK_LOG_ERROR("create_renderer Failed to create swapchain");
        free(renderer);
        return nullptr;
    }

    RHIQueueDesc queue_desc = {
        .capabilities = RHI_QUEUE_GRAPHICS,
        .index = 0
    };
    renderer->queue = rhi.get_queue(renderer->device, &queue_desc);
    if (!renderer->queue) {
        SK_LOG_ERROR("create_renderer Failed to initialize pipeline");
        free(renderer);
        return nullptr;
    }

    /*
    std::vector<u8> vertex_ir;
    std::vector<u8> pixel_ir;
    RHIRasterDesc raster_description = {};
    renderer->pipeline = rhi.create_graphics_pipeline(renderer->device, vertex_ir.data(), vertex_ir.size(), pixel_ir.data(), pixel_ir.size(), &raster_description);
    if (!renderer->pipeline) {
        SK_LOG_ERROR("create_renderer Failed to initialize pipeline");
        free(renderer);
        return nullptr;
    }
    */

    renderer->next_frame = 1;

    renderer->semaphore = rhi.create_semaphore(renderer->device, 0);
    if (!renderer->semaphore) {
        SK_LOG_ERROR("create_renderer Failed to create semaphore");
        free(renderer);
        return nullptr;
    }

    return renderer;
}

void destroy_renderer(Renderer *renderer) {
    rhi.device_wait_idle(renderer->device);
    //rhi.destroy_pipeline(renderer->device, renderer->pipeline);
    rhi.destroy_semaphore(renderer->device, renderer->semaphore);
    rhi.destroy_swapchain(renderer->device, renderer->swapchain);
    rhi.destroy_device(renderer->device);

    free(renderer);
}

bool render(Renderer *renderer) {
    if (renderer->next_frame > renderer->max_frames_in_flight) {
        rhi.wait_semaphore(renderer->device, renderer->semaphore, renderer->next_frame - renderer->max_frames_in_flight);
    }

    RHITexture backbuffer = rhi.next_backbuffer(renderer->swapchain);

    RHICommandBuffer cb = rhi.start_command_recording(renderer->queue);
    {
        RHIRenderPassAttachment color_attachment = {
            .texture = backbuffer,
            .clear = true,
            .clear_value = {
                .type = RHI_CLEAR_VALUE_TYPE_COLOR,
                .color = { 1.0f, 1.0f, 0.0f, 1.0f },
            }
        };

        RHIRenderPassDesc rp = {
            .color_attachments = &color_attachment,
            .color_attachment_count = 1,
        };

        rhi.begin_render_pass(cb, &rp);
        {
            /*
            rhi.set_pipeline(cb, renderer->pipeline);
            rhi.draw_indexed_instanced(cb, nullptr, nullptr, nullptr, 0, 0);
            */
        }
        rhi.end_render_pass(cb);
    }
    rhi.submit(renderer->queue, &cb, 1, renderer->semaphore, renderer->next_frame++);

    rhi.present(renderer->swapchain, backbuffer);

    return true;
}
