#pragma once

#include "Buffer.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

#define MAX_QUADS 1000
#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define MAX_TEXTURE_SLOTS 32

// TODO rework
// Does not need to be a class
namespace sk {
void renderer_2d_init();
void renderer_2d_shutdown();

void renderer_2d_begin_scene(const Camera &camera);
void renderer_2d_begin_scene(const Camera &camera, const std::array<std::array<f32, 4>, 4> &position);
void renderer_2d_end_scene();

void renderer_2d_draw_quad();
void renderer_2d_draw_circle();
void renderer_2d_draw_triangle();
void renderer_2d_draw_line();
void renderer_2d_draw_sprite();
void renderer_2d_draw_string();

void renderer_2d_flush();
void renderer_2d_start_batch();
void renderer_2d_next_batch();
} // namespace SK
