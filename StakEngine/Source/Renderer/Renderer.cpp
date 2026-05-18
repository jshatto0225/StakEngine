#include "Renderer.h"

#include "Window.h"
#include "Platform.h"
#include "SK.h"
#include "Log.h"

Renderer *create_renderer(Window win) {
    auto renderer = (Renderer *) malloc(sizeof(Renderer));

    if (!renderer) {
        SK_LOG_ERROR("create_renderer malloc failed");
        return nullptr;
    }

    sk_load();

    if (!sk_init(nullptr, nullptr, [](const char *msg, SKSeverity severity) { 
        switch (severity) {
            case SK_SEVERITY_TRACE:
                SK_LOG_TRACE(msg);
                break;
            case SK_SEVERITY_INFO:
                SK_LOG_INFO(msg);
                break;
            case SK_SEVERITY_WARN:
                SK_LOG_WARN(msg);
                break;
            case SK_SEVERITY_ERROR:
                SK_LOG_ERROR(msg);
                break;
            case SK_SEVERITY_CRITICAL:
                SK_LOG_CRITICAL(msg);
                break;
        }
    })) {
        SK_LOG_ERROR("create_renderer sk_init failed");
        free(renderer);
        return nullptr;
    }

    renderer->window = win;

    SKQueueRequest queue_request = {
        .capabilities = SK_QUEUE_GRAPHICS,
        .count = 1
    };
    SKDeviceDesc device_desc = {
        .queues = &queue_request,
        .queue_count = 1
    };
    renderer->device = sk_create_device(&device_desc);
    if (!renderer->device) {
        SK_LOG_ERROR("create_renderer Failed to create sk device");
        free(renderer);
        return nullptr;
    }

    SKSwapchainDesc swapchain_desc = {
        .platform = SK_PLATFORM_WIN32,
        .width = platform_get_window_width(win),
        .height = platform_get_window_height(win),
        .format = SK_FORMAT_RGBA8_SRGB,
        .image_count = renderer->max_frames_in_flight,
        .vsync = true,
        .window = platform_get_window_handle(win),
    };
    renderer->swapchain = sk_create_swapchain(renderer->device, &swapchain_desc);
    if (!renderer->swapchain) {
        SK_LOG_ERROR("create_renderer Failed to create swapchain");
        free(renderer);
        return nullptr;
    }

    SKQueueDesc queue_desc = {
        .capabilities = SK_QUEUE_GRAPHICS,
        .index = 0
    };
    renderer->queue = sk_get_queue(renderer->device, &queue_desc);
    if (!renderer->queue) {
        SK_LOG_ERROR("create_renderer Failed to initialize pipeline");
        free(renderer);
        return nullptr;
    }

    /*
    std::vector<u8> vertex_ir;
    std::vector<u8> pixel_ir;
    SKRasterDesc raster_description = {};
    renderer->pipeline = sk_create_graphics_pipeline(renderer->device, vertex_ir.data(), vertex_ir.size(), pixel_ir.data(), pixel_ir.size(), &raster_description);
    if (!renderer->pipeline) {
        SK_LOG_ERROR("create_renderer Failed to initialize pipeline");
        free(renderer);
        return nullptr;
    }
    */

    renderer->next_frame = 1;

    renderer->semaphore = sk_create_semaphore(renderer->device, 0);
    if (!renderer->semaphore) {
        SK_LOG_ERROR("create_renderer Failed to create semaphore");
        free(renderer);
        return nullptr;
    }

    return renderer;
}

void destroy_renderer(Renderer *renderer) {
    sk_device_wait_idle(renderer->device);
    //sk.destroy_pipeline(renderer->device, renderer->pipeline);
    sk_destroy_semaphore(renderer->device, renderer->semaphore);
    sk_destroy_swapchain(renderer->device, renderer->swapchain);
    sk_destroy_device(renderer->device);

    sk_shutdown();

    sk_unload();

    free(renderer);
}

bool render(Renderer *renderer) {
    if (renderer->next_frame > renderer->max_frames_in_flight) {
        sk_wait_semaphore(renderer->device, renderer->semaphore, renderer->next_frame - renderer->max_frames_in_flight);
    }

    SKTexture backbuffer = sk_next_backbuffer(renderer->swapchain);

    SKCommandBuffer cb = sk_start_command_recording(renderer->queue);
    {
        SKRenderPassAttachment color_attachment = {
            .texture = backbuffer,
            .clear = true,
            .clear_value = {
                .type = SK_CLEAR_VALUE_TYPE_COLOR,
                .color = { 1.0f, 1.0f, 0.0f, 1.0f },
            }
        };

        SKRenderPassDesc rp = {
            .color_attachments = &color_attachment,
            .color_attachment_count = 1,
        };

        sk_begin_render_pass(cb, &rp);
        {
            /*
            sk_set_pipeline(cb, renderer->pipeline);
            sk_draw_indexed_instanced(cb, nullptr, nullptr, nullptr, 0, 0);
            */
        }
        sk_end_render_pass(cb);
    }
    sk_submit(renderer->queue, &cb, 1, renderer->semaphore, renderer->next_frame++);

    sk_present(renderer->swapchain, backbuffer);

    return true;
}
