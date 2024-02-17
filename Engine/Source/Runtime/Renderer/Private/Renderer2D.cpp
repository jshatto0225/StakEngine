#include "Renderer2D.h"

#include "Log.h"
#include "PlatformManager.h"
#include "RenderCommand.h"

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

static Renderer2DData data;

void renderer_2d_init() {
  // QUADS
  data.quad_vertex_array = new_vertex_array();
  data.quad_vertex_buffer = new_vertex_buffer(data.max_vertices * sizeof(QuadVertex));
  data.quad_vertex_buffer->set_layout({
    {ShaderDataType::FLOAT3, "aPosition", false},
    {ShaderDataType::FLOAT4, "aColor", false},
    {ShaderDataType::FLOAT2, "aTextureCoordinate", false},
    {ShaderDataType::FLOAT, "aTextureIndex", false},
    {ShaderDataType::FLOAT, "aTilingFactor", false}
    });
  data.quad_vertex_array->add_vertex_buffer(data.quad_vertex_buffer);
  data.quad_vertex_buffer_base = new QuadVertex[data.max_vertices];
  u32 *quad_indices = new u32[data.max_indices];
  u32 offset = 0;
  for (u32 i = 0; i < data.max_indices; i += INDICES_PER_QUAD) {
    quad_indices[i + 0] = offset + 0;
    quad_indices[i + 1] = offset + 1;
    quad_indices[i + 2] = offset + 2;

    quad_indices[i + 3] = offset + 2;
    quad_indices[i + 4] = offset + 3;
    quad_indices[i + 5] = offset + 0;

    offset += VERTICES_PER_QUAD;
  }
  Shared<IndexBuffer> quad_index_buffer = new_index_buffer(quad_indices, data.max_indices);
  data.quad_vertex_array->set_index_buffer(quad_index_buffer);
  delete[] quad_indices;

  // CIRCLES
  data.circle_vertex_array = new_vertex_array();
  data.circle_vertex_buffer = new_vertex_buffer(data.max_vertices * sizeof(CircleVertex));
  data.circle_vertex_buffer->set_layout({
    {ShaderDataType::FLOAT3, "aWorldPosition", false},
    {ShaderDataType::FLOAT3, "aLocalPosition", false},
    {ShaderDataType::FLOAT4, "aColor", false},
    {ShaderDataType::FLOAT, "aThickness", false},
    {ShaderDataType::FLOAT, "aFade", false}
    });
  data.circle_vertex_array->add_vertex_buffer(data.circle_vertex_buffer);
  data.circle_vertex_array->set_index_buffer(quad_index_buffer);
  data.circle_vertex_buffer_base = new CircleVertex[data.max_vertices];

  // LINES
  data.line_vertex_array = new_vertex_array();
  data.line_vertex_buffer = new_vertex_buffer(data.max_vertices * sizeof(LineVertex));
  data.line_vertex_buffer->set_layout({
    {ShaderDataType::FLOAT3, "aPosition", false},
    {ShaderDataType::FLOAT4, "aColor", false}
    });
  data.line_vertex_array->add_vertex_buffer(data.line_vertex_buffer);
  data.line_vertex_buffer_base = new LineVertex[data.max_vertices];
  data.line_width = 2.0f;

  // TEXT
  data.text_vertex_array = new_vertex_array();
  data.text_vertex_buffer = new_vertex_buffer(data.max_vertices * sizeof(TextVertex));
  data.text_vertex_buffer->set_layout({
    {ShaderDataType::FLOAT3, "aPosition", false},
    {ShaderDataType::FLOAT4, "aColor", false},
    {ShaderDataType::FLOAT2, "aTextureCoordinate", false}
    });
  data.text_vertex_array->add_vertex_buffer(data.text_vertex_buffer);
  data.text_vertex_array->set_index_buffer(quad_index_buffer);
  data.text_vertex_buffer_base = new TextVertex[data.max_vertices];

  data.white_texture = new_texture_2d(TextureSpecification());
  u32 white_texture_data = 0xffffffff;
  data.white_texture->set_data(&white_texture_data, sizeof(u32));

  i32 samplers[data.max_texture_slots];
  for (u32 i = 0; i < data.max_texture_slots; i++) {
    samplers[i] = i;
  }

  data.quad_shader = new_shader("Quad2DVertex.glsl", "Quad2DFragment.glsl");
  data.circle_shader = new_shader("Circle2DVertex.glsl", "Circle2DFragment.glsl");
  data.line_shader = new_shader("Line2DVertex.glsl", "Line2DFragment.glsl");
  data.text_shader = new_shader("Text2DVertex.glsl", "Text2DFragment.glsl");

  data.texture_slots[0] = data.white_texture;
  data.texture_slot_index = 1;

  data.quad_vertex_positions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
  data.quad_vertex_positions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
  data.quad_vertex_positions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };
  data.quad_vertex_positions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };
  // sData.cameraUniformBuffer =
  // PlatformManager::NewUniformBuffer(sizeof(Renderer2DData::CameraData), 0);
}

void renderer_2d_shutdown() {
  data.quad_shader.reset();
  data.circle_shader.reset();
  data.line_shader.reset();
  data.text_shader.reset();

  data.white_texture.reset();

  data.quad_vertex_array.reset();
  data.quad_vertex_buffer.reset();

  data.circle_vertex_array.reset();
  data.circle_vertex_buffer.reset();

  data.line_vertex_array.reset();
  data.line_vertex_buffer.reset();

  data.text_vertex_array.reset();
  data.text_vertex_buffer.reset();

  delete[] data.quad_vertex_buffer_base;
  delete[] data.circle_vertex_buffer_base;
  delete[] data.line_vertex_buffer_base;
  delete[] data.text_vertex_buffer_base;
}

void renderer_2d_begin_scene(const Camera &camera) {
  // Start Camera
  data.camera_buffer.view_proj = camera.get_view_projection();
  renderer_2d_start_batch();
}

void renderer_2d_begin_scene(const Camera &camera, const std::array<std::array<f32, 4>, 4> &position) {
  // Start Camera
  // sData.cameraBuffer.viewProj = camera.GetProjection() * Inverse(position);
  renderer_2d_start_batch();
  SK_CORE_WARN("Begin Scene With Position Parameter not implemented");
}

void renderer_2d_end_scene() {
  renderer_2d_flush();
}

void renderer_2d_draw_quad() {}

void renderer_2d_draw_circle() {}

void renderer_2d_draw_triangle() {}

void renderer_2d_draw_line() {}

void renderer_2d_draw_sprite() {}

void renderer_2d_draw_string() {}

void renderer_2d_flush() {
  if (data.quad_index_count) {
    // These two lines tell the renderer how many vertices to bind and render in this pass
    u32 data_size = (u32)((u8 *)data.quad_vertex_buffer_ptr - (u8 *)data.quad_vertex_buffer_base); // Get Size of Quad Vertex Data using ptr math
    data.quad_vertex_buffer->set_data(data.quad_vertex_buffer_base, data_size); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

    // Bind All Textures
    for (u32 i = 0; i < data.texture_slot_index; i++) {
      data.texture_slots[i]->bind(i);
    }

    data.quad_shader->bind();
    // Draw quadIndexCount quads fron quadVertexArray
    render_command_draw_indexed(data.quad_vertex_array, data.quad_index_count);
  }

  if (data.circle_index_count) {
    // These two lines tell the renderer how many vertices to bind and render in this pass
    u32 data_size = (u32)((u8 *)data.circle_vertex_buffer_ptr - (u8 *)data.circle_vertex_buffer_base); // Get Size of Quad Vertex Data using ptr math
    data.circle_vertex_buffer->set_data(data.circle_vertex_buffer_base, data_size); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

    // Bind All Textures
    for (u32 i = 0; i < data.texture_slot_index; i++) {
      data.texture_slots[i]->bind(i);
    }

    data.circle_shader->bind();
    // Draw quadIndexCount quads fron quadVertexArray
    render_command_draw_indexed(data.circle_vertex_array, data.circle_index_count);
  }

  if (data.line_vertex_count) {
    // These two lines tell the renderer how many vertices to bind and render in this pass
    u32 data_size = (u32)((u8 *)data.line_vertex_buffer_ptr - (u8 *)data.line_vertex_buffer_base); // Get Size of Quad Vertex Data using ptr math
    data.line_vertex_buffer->set_data(data.line_vertex_buffer_base, data_size); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

    // Bind All Textures
    for (u32 i = 0; i < data.texture_slot_index; i++) {
      data.texture_slots[i]->bind(i);
    }

    data.line_shader->bind();
    // Draw quadIndexCount quads fron quadVertexArray
    render_command_set_line_width(data.line_width);
    render_command_draw_lines(data.line_vertex_array, data.line_vertex_count);
  }

  if (data.text_index_count) {
    // These two lines tell the renderer how many vertices to bind and render in this pass
    u32 data_size = (u32)((u8 *)data.text_vertex_buffer_ptr - (u8 *)data.text_vertex_buffer_base); // Get Size of Quad Vertex Data using ptr math
    data.text_vertex_buffer->set_data(data.text_vertex_buffer_base, data_size); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

    // Bind All Textures
    for (u32 i = 0; i < data.texture_slot_index; i++) {
      data.texture_slots[i]->bind(i);
    }

    data.text_shader->bind();
    // Draw quadIndexCount quads fron quadVertexArray
    render_command_draw_indexed(data.text_vertex_array, data.text_index_count);
  }
}

void renderer_2d_start_batch() {
  data.quad_index_count = 0;
  data.quad_vertex_buffer_ptr = data.quad_vertex_buffer_base;

  data.circle_index_count = 0;
  data.circle_vertex_buffer_ptr = data.circle_vertex_buffer_base;

  data.line_vertex_count = 0;
  data.line_vertex_buffer_ptr = data.line_vertex_buffer_base;

  data.text_index_count = 0;
  data.text_vertex_buffer_ptr = data.text_vertex_buffer_base;

  data.texture_slot_index = 1;
}

void renderer_2d_next_batch() {
  renderer_2d_flush();
  renderer_2d_start_batch();
}
} // namespace SK
