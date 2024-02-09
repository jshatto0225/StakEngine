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

namespace sk {
struct QuadVertex {
  std::array<f32, 3> position;
  std::array<f32, 4> color;
  std::array<f32, 2> texture_coordinate;
  f32 texture_index;
  f32 tiling_factor;
};

struct CircleVertex {
  std::array<f32, 3> world_position;
  std::array<f32, 3> local_position;
  std::array<f32, 4> color;
  f32 thickness;
  f32 fade;
};

struct LineVertex {
  std::array<f32, 3> position;
  std::array<f32, 4> color;
};

struct TextVertex {
  std::array<f32, 3> position;
  std::array<f32, 4> color;
  std::array<f32, 2> texture_coordinate;
};

struct Renderer2DData {
  static const u32 max_quads = MAX_QUADS;
  static const u32 max_vertices = max_quads * VERTICES_PER_QUAD;
  static const u32 max_indices = max_quads * INDICES_PER_QUAD;
  static const u32 max_texture_slots = MAX_TEXTURE_SLOTS;

  Shared<VertexArray> quad_vertex_array;
  Shared<VertexBuffer> quad_vertex_buffer;
  Shared<Shader> quad_shader;

  Shared<VertexArray> circle_vertex_array;
  Shared<VertexBuffer> circle_vertex_buffer;
  Shared<Shader> circle_shader;

  Shared<VertexArray> line_vertex_array;
  Shared<VertexBuffer> line_vertex_buffer;
  Shared<Shader> line_shader;

  Shared<VertexArray> text_vertex_array;
  Shared<VertexBuffer> text_vertex_buffer;
  Shared<Shader> text_shader;

  QuadVertex *quad_vertex_buffer_base;
  QuadVertex *quad_vertex_buffer_ptr;
  u32 quad_index_count;

  CircleVertex *circle_vertex_buffer_base;
  CircleVertex *circle_vertex_buffer_ptr;
  u32 circle_index_count;

  LineVertex *line_vertex_buffer_base;
  LineVertex *line_vertex_buffer_ptr;
  u32 line_vertex_count;

  TextVertex *text_vertex_buffer_base;
  TextVertex *text_vertex_buffer_ptr;
  u32 text_index_count;

  Shared<Texture2D> white_texture;

  f32 line_width;

  std::array<Shared<Texture2D>, max_texture_slots> texture_slots;
  u32 texture_slot_index;

  std::array<std::array<f32, 4>, 4> quad_vertex_positions;

  struct CameraBuffer {
    std::array<std::array<f32, 4>, 4> view_proj;
  };
  CameraBuffer camera_buffer;
};

class Renderer2D final {
public:
  static void init();
  static void shutdown();

  static void begin_scene(const Camera &camera);
  static void begin_scene(const Camera &camera,
                         const std::array<std::array<f32, 4>, 4> &position);
  static void end_scene();

  static void draw_quad();
  static void draw_circle();
  static void draw_triangle();
  static void draw_line();
  static void draw_sprite();
  static void draw_string();

  static void flush();
  static void start_batch();
  static void next_batch();

private:
  static Renderer2DData data;
};
} // namespace SK
