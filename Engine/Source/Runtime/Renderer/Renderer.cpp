#include "Renderer.h"

#include <iostream>

#include "Log.h"
#include "StakMath.h"

u32 ShaderDataTypeSize(ShaderDataType type) {
  switch (type) {
  case ShaderDataType::FLOAT:
    return 4;
  case ShaderDataType::FLOAT2:
    return 4 * 2;
  case ShaderDataType::FLOAT3:
    return 4 * 3;
  case ShaderDataType::FLOAT4:
    return 4 * 4;
  case ShaderDataType::MAT3:
    return 4 * 3 * 3;
  case ShaderDataType::MAT4:
    return 4 * 4 * 4;
  case ShaderDataType::INT:
    return 4;
  case ShaderDataType::INT2:
    return 4 * 2;
  case ShaderDataType::INT3:
    return 4 * 3;
  case ShaderDataType::INT4:
    return 4 * 4;
  case ShaderDataType::BOOL:
    return 1;
  }
  return 0;
}

BufferElement::BufferElement(ShaderDataType type, const std::string &name, bool normalized) {
  this->name = name.c_str();
  this->type = type;
  this->normalized = normalized;
  this->offset = 0;
  this->size = ShaderDataTypeSize(type);
}

u32 BufferElement::getComponentCount() const {
  switch (this->type) {
    case ShaderDataType::FLOAT:
      return 1;
    case ShaderDataType::FLOAT2:
      return 2;
    case ShaderDataType::FLOAT3:
      return 3;
    case ShaderDataType::FLOAT4:
      return 4;
    case ShaderDataType::MAT3:
      return 3;
    case ShaderDataType::MAT4:
      return 4;
    case ShaderDataType::INT:
      return 1;
    case ShaderDataType::INT2:
      return 2;
    case ShaderDataType::INT3:
      return 3;
    case ShaderDataType::INT4:
      return 4;
    case ShaderDataType::BOOL:
      return 1;
    default:
      return 0;
  }
}

const char *BufferElement::getName() const {
  return this->name;
}

ShaderDataType BufferElement::getType() const {
  return this->type;
}

u32 BufferElement::getSize() const {
  return this->size;
}

u64 BufferElement::getOffset() const {
  return this->offset;
}

bool BufferElement::isNormalized() const {
  return this->normalized;
}

BufferLayout::BufferLayout() {
  this->stride = 0;
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements) {
  this->elements = elements;
  u64 offset = 0;
  this->stride = 0;
  for (BufferElement &element : this->elements) {
    element.offset = offset;
    offset += element.getSize();
    stride += element.getSize();
  }
}

u32 BufferLayout::getStride() const {
  return this->stride;
}

const std::vector<BufferElement> BufferLayout::getElements() const {
  return this->elements;
}

std::vector<BufferElement>::iterator BufferLayout::begin() {
  return elements.begin();
}

std::vector<BufferElement>::iterator BufferLayout::end() {
  return elements.end();
}

std::vector<BufferElement>::const_iterator BufferLayout::begin() const {
  return elements.begin();
}

std::vector<BufferElement>::const_iterator BufferLayout::end() const {
  return elements.end();
}

void RenderCommand::Init() {
  RenderApi::Init();
}

void RenderCommand::Shutdown() {
  RenderApi::Shutdown();
}

void RenderCommand::SetClearColor(f32 r, f32 g, f32 b, f32 a) {
  RenderApi::SetClearColor(r, g, b, a);
}

void RenderCommand::SetViewport(i32 x, i32 y, i32 width, i32 height) {
  RenderApi::SetViewport(x, y, width, height);
}

void RenderCommand::SetLineWidth(f32 width) {
  RenderApi::SetLineWidth(width);
}

void RenderCommand::Bind() {
  RenderApi::Bind();
}

void RenderCommand::Clear() {
  RenderApi::Clear();
}

void RenderCommand::DrawIndexed(Ref<VertexArray> vao, u32 count) {
  RenderApi::DrawIndexed(vao, count);
}

void RenderCommand::DrawLines(Ref<VertexArray> vao, u32 count) {
  RenderApi::DrawLines(vao, count);
}

void Renderer::Init() {
  RenderCommand::Init();
  Renderer2D::Init();
}

void Renderer::Shutdown() {
  Renderer2D::Shutdown();
  RenderApi::Shutdown();
}

void Renderer::OnWindowResize(i32 width, i32 height) {
  RenderApi::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene() {

}

void Renderer::EndScene() {

}

void Renderer::Submit(Ref<Shader> shader, Ref<VertexArray> vao) {
  shader->bind();
  vao->bind();
  RenderCommand::DrawIndexed(vao, vao->getIndexBuffer()->getCount());
}

Renderer2D::Renderer2DData Renderer2D::data;

void Renderer2D::Init() {
  u32 *quad_indices = new u32[Renderer2D::data.max_indices];
  u32 offset = 0;
  for (u32 i = 0; i < Renderer2D::data.max_indices; i += Renderer2D::data.indices_per_quad) {
    quad_indices[i + 0] = offset;
    quad_indices[i + 1] = offset + 1;
    quad_indices[i + 2] = offset + 3;
    quad_indices[i + 3] = offset + 1;
    quad_indices[i + 4] = offset + 2;
    quad_indices[i + 5] = offset + 3;
    offset += Renderer2D::data.vertices_per_quad;
  }

  Renderer2D::data.quad_index_count = 0;
  Renderer2D::data.quad_vertices_base = new QuadVertex[Renderer2D::data.max_quad_vertices];
  Renderer2D::data.quad_vertex_current = Renderer2D::data.quad_vertices_base;

  Renderer2D::data.quad_shader = MakeRef<Shader>(SHADER_DIR "QuadShader.glsl");

  Renderer2D::data.quad_vertex_buffer = MakeRef<VertexBuffer>(sizeof(QuadVertex) * Renderer2D::data.max_quad_vertices);
  Renderer2D::data.quad_vertex_buffer->setLayout({ {ShaderDataType::FLOAT3, "aPosition",  false},
                                                   {ShaderDataType::FLOAT2, "aTexCoord",  false},
                                                   {ShaderDataType::FLOAT,    "aTexIndex",  false} });


  Ref<IndexBuffer> quad_index_buffer = MakeRef<IndexBuffer>(quad_indices, Renderer2D::data.max_indices);
  delete[] quad_indices;

  Renderer2D::data.quad_vertex_array = MakeRef<VertexArray>();
  Renderer2D::data.quad_vertex_array->addVertexBuffer(Renderer2D::data.quad_vertex_buffer);
  Renderer2D::data.quad_vertex_array->setIndexBuffer(quad_index_buffer);

  Renderer2D::data.texture_slot_index = 1;
  Renderer2D::data.white_texture = MakeRef<Texture2D>(TextureSpecification());
  u32 white_color = 0xffffffff;
  Renderer2D::data.white_texture->setData(&white_color, sizeof(white_color));
  Renderer2D::data.texture_slots[0] = Renderer2D::data.white_texture;
  Renderer2D::data.cam_uniform_buffer = MakeRef<UniformBuffer>(sizeof(Renderer2D::Renderer2DData::CameraData), 0);

  Renderer2D::data.quad_vertex_positions[0] = {  0.5f,  0.5f, 0.0f};
  Renderer2D::data.quad_vertex_positions[1] = {  0.5f, -0.5f, 0.0f};
  Renderer2D::data.quad_vertex_positions[2] = { -0.5f, -0.5f, 0.0f};
  Renderer2D::data.quad_vertex_positions[3] = { -0.5f,  0.5f, 0.0f};

  Renderer2D::data.quad_tex_coords[0] = { 1.0f, 1.0f };
  Renderer2D::data.quad_tex_coords[1] = { 1.0f, 0.0f };
  Renderer2D::data.quad_tex_coords[2] = { 0.0f, 0.0f };
  Renderer2D::data.quad_tex_coords[3] = { 0.0f, 1.0f };
}

void Renderer2D::Shutdown() {
  delete[] Renderer2D::data.quad_vertices_base;
}

void Renderer2D::DrawQuad(const Vec2 &pos, const Vec2 &size, const Vec4 &color) {
  Renderer2D::DrawQuad(pos, size, 0, color);
}

void Renderer2D::DrawQuad(const Vec2 &pos, const Vec2 &size, const Ref<Texture2D> &tex) {
  Renderer2D::DrawQuad(pos, size, 0, tex);
}

void Renderer2D::DrawQuad(const Vec2 &pos, const Vec2 &size, f32 rotation, const Vec4 &color) {
  Ref<Texture2D> texture = MakeRef<Texture2D>(TextureSpecification());
  i32 r = std::min(std::max(color.r, 0.0f), 1.0f) * 255;
  i32 g = std::min(std::max(color.g, 0.0f), 1.0f) * 255;
  i32 b = std::min(std::max(color.b, 0.0f), 1.0f) * 255;
  i32 a = std::min(std::max(color.a, 0.0f), 1.0f) * 255;
  u32 c = (a << 24) | (b << 16) | (g << 8) | (r << 0);
  texture->setData((void *)&c, sizeof(c));
  Renderer2D::DrawQuad(pos, size, rotation, texture);
}

void Renderer2D::DrawQuad(const Vec2 &pos, const Vec2 &size, f32 rotation, const Ref<Texture2D> &tex) {
  Mat4 transform = TranstationMatrix(Vec3(pos, 0.0f)) * RotationMatrix2D(rotation) * ScaleMatrix(Vec3(size, 1.0f));
  Renderer2D::DrawQuad(transform, tex);
}

void Renderer2D::DrawQuad(const Mat4 &transform, const Ref<Texture2D> &tex) {
  if (Renderer2D::data.texture_slot_index == Renderer2D::data.max_texture_slots ||
      Renderer2D::data.quad_index_count == Renderer2D::data.max_quad_vertices) {
    Renderer2D::NextBatch();
  }

  f32 slot = -1;
  for (u32 i = 0; i < Renderer2D::data.texture_slot_index; i++) {
    if (*Renderer2D::data.texture_slots[i] == *tex) {
      slot = i;
      break;
    }
  }
  if (slot == -1) {
    slot = Renderer2D::data.texture_slot_index;
    Renderer2D::data.texture_slots[Renderer2D::data.texture_slot_index] = tex;
    Renderer2D::data.texture_slot_index++;
  }

  for (u32 i = 0; i < Renderer2D::data.vertices_per_quad; i++) {
    Renderer2D::data.quad_vertex_current->pos = transform * Vec4(Renderer2D::data.quad_vertex_positions[i], 1.0f);
    Renderer2D::data.quad_vertex_current->tex_coord = Renderer2D::data.quad_tex_coords[i];
    Renderer2D::data.quad_vertex_current->tex_index = slot;
    Renderer2D::data.quad_vertex_current++;
  }

  Renderer2D::data.quad_index_count += Renderer2D::data.indices_per_quad;
}

void Renderer2D::DrawCircle(const Vec2 &pos, f32 radius, const Vec4 &color) {

}

void Renderer2D::DrawCircle(const Vec2 &pos, f32 radius, const Ref<Texture2D> &tex) {

}

void Renderer2D::DrawLine(const Vec2 &p1, const Vec2 &p2, const Vec4 &color) {

}

void Renderer2D::DrawText(const std::string &text, const Ref<Font> &font, const Vec2 &pos) {

}

void Renderer2D::Flush() {
  if (Renderer2D::data.quad_index_count) {
    u32 data_size = (u32)((u8 *)Renderer2D::data.quad_vertex_current - (u8 *)Renderer2D::data.quad_vertices_base);
    Renderer2D::data.quad_vertex_buffer->setData(Renderer2D::data.quad_vertices_base, data_size);

    for (u32 i = 0; i < Renderer2D::data.texture_slot_index; i++) {
      Renderer2D::data.texture_slots[i]->bind(i);
    }

    Renderer2D::data.quad_shader->bind();

    RenderCommand::DrawIndexed(Renderer2D::data.quad_vertex_array, Renderer2D::data.quad_index_count);
  }
}

void Renderer2D::BeginScene(const SceneViewCamera &cam) {
  Renderer2D::data.cam_data.view_proj = cam.getViewProj();
  Renderer2D::data.cam_uniform_buffer->setData(&Renderer2D::data.cam_data.view_proj, 
                                               sizeof(Renderer2D::Renderer2DData::CameraData));
  Renderer2D::StartBatch();
}

void Renderer2D::StartBatch() {
  Renderer2D::data.quad_index_count = 0;
  Renderer2D::data.quad_vertex_current = Renderer2D::data.quad_vertices_base;

  Renderer2D::data.texture_slot_index = 1;
}

void Renderer2D::NextBatch() {
  Renderer2D::Flush();
  Renderer2D::StartBatch();
}

void Renderer2D::EndScene() {
  Renderer2D::Flush();
}
