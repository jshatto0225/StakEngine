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
void sk_renderer_2d_init();
void sk_renderer_2d_shutdown();

void sk_renderer_2d_begin_scene(const Camera &camera);
void sk_renderer_2d_begin_scene(const Camera &camera, const std::array<std::array<f32, 4>, 4> &position);
void sk_renderer_2d_end_scene();

void sk_renderer_2d_draw_quad();
void sk_renderer_2d_draw_circle();
void sk_renderer_2d_draw_triangle();
void sk_renderer_2d_draw_line();
void sk_renderer_2d_draw_sprite();
void sk_renderer_2d_draw_string();

void sk_renderer_2d_flush();
void sk_renderer_2d_start_batch();
void sk_renderer_2d_next_batch();

