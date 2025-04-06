#include "Renderer.h"

#include "Window.h"
#include "RHI.h"
#include "Platform.h"
#include "Log.h"

bool renderer_init(Renderer *renderer, Window *Win, bool render_to_offscreen_buffer) {
    renderer->window = Win;
    renderer->use_offscreen_buffer = render_to_offscreen_buffer;

    renderer->swapchain_texture = rhi.get_current_swapchain_texture(renderer->window->swapchain);

    Rhi_Command_List_Description command_list_description = {};
    command_list_description.is_secondary = false;
    renderer->command_list = rhi.create_command_list(&command_list_description);
    if (!renderer->command_list) {
        SK_LOG_ERROR("Failed to initialize command context");
        return false;
    }

    if (!renderer->use_offscreen_buffer) {
        Rhi_Shader_Description vertex_shader_description = {};
        vertex_shader_description.name = "BasicShader.vert";
        vertex_shader_description.type = Rhi_Shader_Type::VERTEX;
        Rhi_Resource_Handle vertex_shader = rhi.create_shader(&vertex_shader_description);
        if (!vertex_shader) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        Rhi_Shader_Description fragment_shader_description = {};
        fragment_shader_description.name = "BasicShader.frag";
        fragment_shader_description.type = Rhi_Shader_Type::FRAGMENT;
        Rhi_Resource_Handle fragment_shader = rhi.create_shader(&fragment_shader_description);
        if (!fragment_shader) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        Rhi_Pipeline_Layout_Description pipeline_layout_description = {};
        renderer->pipeline_layout = rhi.create_pipeline_layout(&pipeline_layout_description);
        if (!renderer->pipeline_layout) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        Rhi_Resource_Handle shaders[] = { vertex_shader, fragment_shader };
        Rhi_Format formats[] = { rhi.get_texture_format(renderer->swapchain_texture) };

        Rhi_Graphics_Pipeline_State_Description pipeline_description = {};
        pipeline_description.color_formats = formats;
        pipeline_description.depth_stencil_format = { Rhi_Format::UNDEFINED };
        pipeline_description.layout = renderer->pipeline_layout;
        pipeline_description.shader_count = 2;
        pipeline_description.shaders = shaders;
        pipeline_description.vertex_input_attributes = {};
        pipeline_description.vertex_input_bindings = {};

        renderer->pipeline = rhi.create_graphics_pipeline(&pipeline_description);
        if (!renderer->pipeline) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        rhi.destroy_shader(&vertex_shader);
        rhi.destroy_shader(&fragment_shader);
    }

    return true;
}

void renderer_shutdown(Renderer *renderer) {
    rhi.destroy_pipeline_layout(&renderer->pipeline_layout);
    rhi.destroy_pipeline(&renderer->pipeline);

    rhi.wait_for_gpu_idle();
    rhi.destroy_command_list(&renderer->command_list);
}

bool renderer_render(Renderer *renderer) {
    rhi.prepare_frame(renderer->window->swapchain);

    // NOTE: There will be a new backbuffer every frame
    renderer->swapchain_texture = rhi.get_current_swapchain_texture(renderer->window->swapchain);

    if (!rhi.begin_command_list(renderer->command_list)) {
        SK_LOG_ERROR("Failed to begin command context");
        return false;
    }
    {

        Rhi_Resource_Barrier render_target_barrier = {};
        render_target_barrier.type = Rhi_Barrier_Type::TRANSITION;
        render_target_barrier.transition_barrier.resource = renderer->swapchain_texture;
        render_target_barrier.transition_barrier.state_before = Rhi_Resource_State::UNDEFINED;
        render_target_barrier.transition_barrier.state_after = Rhi_Resource_State::RENDER_TARGET;
        render_target_barrier.transition_barrier.subresource = 0;

        rhi.cmd_resource_barrier(renderer->command_list, &render_target_barrier);

        // NOTE: Use offscreen backbuffer if requested by the config, else use swapchain backbuffer
        if (renderer->use_offscreen_buffer) {
            Rhi_Resource_Barrier offscreen_buffer_render_target_barrier = {};
            offscreen_buffer_render_target_barrier.type = Rhi_Barrier_Type::TRANSITION;
            offscreen_buffer_render_target_barrier.transition_barrier.resource = renderer->offscreen_backbuffers[renderer->offscreen_backbuffer_image_index];
            offscreen_buffer_render_target_barrier.transition_barrier.state_before = Rhi_Resource_State::UNDEFINED;
            offscreen_buffer_render_target_barrier.transition_barrier.state_after = Rhi_Resource_State::RENDER_TARGET;
            offscreen_buffer_render_target_barrier.transition_barrier.subresource = 0;

            rhi.cmd_resource_barrier(renderer->command_list, &offscreen_buffer_render_target_barrier);


            auto render_area = rhi.get_texture_render_area(renderer->offscreen_backbuffers[renderer->offscreen_backbuffer_image_index]);
            rhi.cmd_set_render_target(renderer->command_list, renderer->offscreen_backbuffers[renderer->offscreen_backbuffer_image_index], &render_area);
        } else {
            auto render_area = rhi.get_texture_render_area(renderer->swapchain_texture);
            rhi.cmd_set_render_target(renderer->command_list, renderer->swapchain_texture, &render_area);
        }

        rhi.cmd_bind_pipeline(renderer->command_list, renderer->pipeline);
        auto [layer_count, x, y, width, height] = rhi.get_texture_render_area(renderer->swapchain_texture);
        rhi.cmd_set_viewport(renderer->command_list, (f32)x, (f32)y, (f32)width, (f32)height, 0.0f, 1.0f);
        rhi.cmd_set_scissor(renderer->command_list, (s32)x, (s32)y, width, height);
        rhi.cmd_draw_instanced(renderer->command_list, 3, 1, 0, 0);

        if (renderer->use_offscreen_buffer) {
            // NOTE: Unset offscreen backbuffer
            rhi.cmd_unset_render_target(renderer->command_list);

            Rhi_Resource_Barrier offscreen_buffer_shader_resource_barrier = {};
            offscreen_buffer_shader_resource_barrier.type = Rhi_Barrier_Type::TRANSITION;
            offscreen_buffer_shader_resource_barrier.transition_barrier.resource = renderer->offscreen_backbuffers[renderer->offscreen_backbuffer_image_index];
            offscreen_buffer_shader_resource_barrier.transition_barrier.state_before = Rhi_Resource_State::RENDER_TARGET;
            offscreen_buffer_shader_resource_barrier.transition_barrier.state_after = Rhi_Resource_State::SHADER_RESOURCE;
            offscreen_buffer_shader_resource_barrier.transition_barrier.subresource = 0;

            rhi.cmd_resource_barrier(renderer->command_list, &offscreen_buffer_shader_resource_barrier);

            auto render_area = rhi.get_texture_render_area(renderer->offscreen_backbuffers[renderer->offscreen_backbuffer_image_index]);
            rhi.cmd_set_render_target(renderer->command_list, renderer->swapchain_texture, &render_area);
        }

        if (renderer->gui_render_func) renderer->gui_render_func(renderer->command_list);

        rhi.cmd_unset_render_target(renderer->command_list);

        Rhi_Resource_Barrier present_barrier = {};
        present_barrier.type = Rhi_Barrier_Type::TRANSITION;
        present_barrier.transition_barrier.resource = renderer->swapchain_texture;
        present_barrier.transition_barrier.state_before = Rhi_Resource_State::RENDER_TARGET;
        present_barrier.transition_barrier.state_after = Rhi_Resource_State::PRESENT;
        present_barrier.transition_barrier.subresource = 0;

        rhi.cmd_resource_barrier(renderer->command_list, &present_barrier);
    }
    if (!rhi.end_command_list(renderer->command_list)) {
        SK_LOG_ERROR("Failed to end command context");
        return false;
    }
    if (!rhi.submit(renderer->command_list)) {
        SK_LOG_ERROR("Failed to submit commands");
        return false;
    }
    if (!rhi.present_frame()) {
        SK_LOG_ERROR("Failed to present");
        return false;
    }

    return true;
}

bool renderer_init_imgui(Renderer *renderer) {
    platform_init_imgui(renderer->window);
    // NOTE: Assume all swapchain textures have the same format
    rhi.init_imgui(renderer->window->swapchain);

    if (renderer->use_offscreen_buffer) {
        Rhi_Texture_Description offscreen_backbuffer_description = {};
        offscreen_backbuffer_description.format = Rhi_Format::B8G8R8A8_SRGB;
        offscreen_backbuffer_description.width = renderer->window->framebuffer_width;
        offscreen_backbuffer_description.height = renderer->window->framebuffer_height;
        offscreen_backbuffer_description.use_for_imgui = true;
        offscreen_backbuffer_description.use_as_shader_resource = true;
        for (auto &backbuffer : renderer->offscreen_backbuffers) {
            backbuffer = rhi.create_texture(&offscreen_backbuffer_description);
            if (!backbuffer) {
                SK_LOG_ERROR("Failed to initialize offscreen backbuffer");
                return false;
            }
        }

        Rhi_Shader_Description vertex_shader_description = {};
        vertex_shader_description.name = "BasicShader.vert";
        vertex_shader_description.type = Rhi_Shader_Type::VERTEX;
        Rhi_Resource_Handle vertex_shader = rhi.create_shader(&vertex_shader_description);
        if (!vertex_shader) {
            SK_LOG_ERROR("Failed to initialize vertex shader");
            return false;
        }

        Rhi_Shader_Description fragment_shader_description = {};
        fragment_shader_description.name = "BasicShader.frag";
        fragment_shader_description.type = Rhi_Shader_Type::FRAGMENT;
        Rhi_Resource_Handle fragment_shader = rhi.create_shader(&fragment_shader_description);
        if (!fragment_shader) {
            SK_LOG_ERROR("Failed to initialize fragment shader");
            return false;
        }

        Rhi_Pipeline_Layout_Description pipeline_layout_description = {};
        renderer->pipeline_layout = rhi.create_pipeline_layout(&pipeline_layout_description);
        if (!renderer->pipeline_layout) {
            SK_LOG_ERROR("Failed to initialize pipeline layout");
            return false;
        }

        Rhi_Format formats[] = { Rhi_Format::B8G8R8A8_SRGB };
        Rhi_Resource_Handle shaders[] = { vertex_shader, fragment_shader };

        Rhi_Graphics_Pipeline_State_Description pipeline_description = {};
        pipeline_description.color_formats = formats;
        pipeline_description.depth_stencil_format = { Rhi_Format::UNDEFINED };
        pipeline_description.layout = renderer->pipeline_layout;
        pipeline_description.shader_count = 2;
        pipeline_description.shaders = shaders;
        pipeline_description.vertex_input_attributes = {};
        pipeline_description.vertex_input_bindings = {};

        renderer->pipeline = rhi.create_graphics_pipeline(&pipeline_description);
        if (!renderer->pipeline) {
            SK_LOG_ERROR("Failed to initialize pipeline");
            return false;
        }

        rhi.destroy_shader(&vertex_shader);
        rhi.destroy_shader(&fragment_shader);
    }

    return true;
}

void renderer_imgui_new_frame(Renderer *renderer) {
    platform_imgui_new_frame();
    rhi.imgui_new_frame();
}

void renderer_shutdown_imgui(Renderer *renderer) {
    rhi.wait_for_gpu_idle();

    for (auto &backbuffer : renderer->offscreen_backbuffers) {
        rhi.destroy_texture(&backbuffer);
    }

    rhi.shutdown_imgui();
    platform_shutdown_imgui();
}

void renderer_add_scene_to_imgui_window(Renderer *renderer) {
    rhi.add_texture_to_imgui_window(renderer->offscreen_backbuffers[renderer->offscreen_backbuffer_image_index]);
}
