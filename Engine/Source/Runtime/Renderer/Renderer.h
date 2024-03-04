#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Types.h"
#include "StakMath.h"
#include "Font.h"

enum class ShaderDataType {
  FLOAT,
  FLOAT2,
  FLOAT3,
  FLOAT4,
  MAT3,
  MAT4,
  INT,
  INT2,
  INT3,
  INT4,
  BOOL
};

u32 ShaderDataTypeSize(ShaderDataType type);

class BufferElement {
public:
  BufferElement(ShaderDataType type, const std::string &name, bool normalized);

  u32 getComponentCount() const;

  const char *getName() const;
  ShaderDataType getType() const;
  u32 getSize() const;
  u64 getOffset() const;
  bool isNormalized() const;

private:
  const char *name;
  ShaderDataType type;
  u32 size;
  u64 offset;
  bool normalized;

  friend class BufferLayout;
};

class BufferLayout {
public:
  BufferLayout();

  BufferLayout(std::initializer_list<BufferElement> elements);

  u32 getStride() const;
  const std::vector<BufferElement> getElements() const;

  std::vector<BufferElement>::iterator begin();
  std::vector<BufferElement>::iterator end();
  std::vector<BufferElement>::const_iterator begin() const;
  std::vector<BufferElement>::const_iterator end() const;

private:
  std::vector<BufferElement> elements;
  u32 stride;
};

class VertexBuffer {
public:
  VertexBuffer(u32 size);
  VertexBuffer(float *vertices, u32 size);
  ~VertexBuffer();

  void bind() const;
  void unbind() const;
  void setData(const void *data, u32 size);

  const BufferLayout &getLayout() const;
  void setLayout(const BufferLayout &layout);

private:
  BufferLayout layout;
  u32 renderer_id;
};

class IndexBuffer {
public:
  IndexBuffer();
  IndexBuffer(u32 *indices, u32 count);
  ~IndexBuffer();

  void bind() const;
  void unbind() const;

  u32 getCount() const;

private:
  u32 count;
  u32 renderer_id;
};

class VertexArray {
public:
  VertexArray();
  ~VertexArray();

  void addVertexBuffer(Shared<VertexBuffer> vbo);
  void setIndexBuffer(Shared<IndexBuffer> ibo);
  void bind() const;
  void unbind() const;

  const std::vector<Shared<VertexBuffer>> &getVertexBuffers() const;
  const Shared<IndexBuffer> getIndexBuffer() const;

private:
  std::vector<Shared<VertexBuffer>> vertex_buffers;
  Shared<IndexBuffer> index_buffer;
  u32 renderer_id;
  u32 vertex_buffer_index;
};

struct ShaderSource {
  std::string vs;
  std::string fs;
};

enum class ShaderType {
  FRAGMENT,
  VERTEX,
  NONE,
};

class Shader {
public:
  Shader(const std::string &vs, const std::string &fs);
  Shader(const std::string &path);
  ~Shader();

  ShaderSource Parse(const std::string &path);
  void compile(const std::string &vs, const std::string &fs);

  void setUniformi(const std::string &name, i32 value);

  void bind() const;
  void unbind() const;

private:
  u32 renderer_id;
};

enum class ImageFormat { 
  R8,
  RGB8,
  RGBA8,
  RGBA32F
};

struct TextureSpecification {
  i32 width = 1;
  i32 height = 1;
  ImageFormat format = ImageFormat::RGBA8;
  bool generate_mips = false;
};

class Texture {
public:
  Texture(const TextureSpecification &textureSpecification);
  ~Texture();

  const TextureSpecification &getSpecification() const;
  u32 getWidth() const;
  u32 getHeight() const;
  u32 getRendererId() const;
  const std::string &getPath() const;
  void setData(void *data, u32 size);
  void bind(u32 slot = 0) const;
  bool isLoaded() const;
  bool operator==(const Texture &other) const;

private:
  TextureSpecification spec;
  std::string path;
  u32 renderer_id;
};

class Texture2D {
public:
  Texture2D(const std::string &path);
  Texture2D(const TextureSpecification &textureSpecification);
  ~Texture2D();

  const TextureSpecification &getSpecification() const;
  u32 getWidth() const;
  u32 getHeight() const;
  u32 getRendererId() const;
  const std::string &getPath() const;
  void setData(void *data, u32 size);
  void bind(u32 slot = 0) const;
  bool isLoaded() const;
  bool operator==(const Texture2D &other) const;

private:
  TextureSpecification spec;
  std::string path;
  u32 renderer_id;
  bool loaded;
};

class RenderApi {
public:
  static void Init();
  static void Shutdown();

  static void SetClearColor(f32 r, f32 g, f32 b, f32 a);
  // NOTE: x and y are relative to the window. (0, 0) is 
  // always the bottom left
  static void SetViewport(i32 x, i32 y, i32 width, i32 height);
  static void SetLineWidth(f32 width);

  static void Bind();
  static void Clear();

 static void DrawIndexed(Shared<VertexArray> vao, u32 count);
 static void DrawLines(Shared<VertexArray> vao, u32 count);

private:
  static bool initialized;
};

class RenderCommand {
public:
  static void Init();
  static void Shutdown();
  static void SetClearColor(f32 r, f32 g, f32 b, f32 a);
  static void SetViewport(i32 x, i32 y, i32 width, i32 height);
  static void SetLineWidth(f32 width);
  static void Bind();
  static void Clear();
  static void DrawIndexed(Shared<VertexArray> vao, u32 count);
  static void DrawLines(Shared<VertexArray> vao, u32 count);
};

class Renderer {
public:
  static void Init();
  static void Shutdown();
  static void OnWindowResize(i32 width, i32 height);
  static void BeginScene();
  static void EndScene();
  static void Submit(Shared<Shader> shader, Shared<VertexArray> vao);
};

class Renderer2D {
public:
  static void Init();
  static void DrawQuad(const Vec2 &pos, const Vec2 &size, const Vec4 &color);
  static void DrawQuad(const Vec2 &pos, const Vec2 &size, const Shared<Texture2D> &tex);
  static void DrawCircle(const Vec2 &pos, f32 radius, const Vec4 &color);
  static void DrawCircle(const Vec2 &pos, f32 radius, const Shared<Texture2D> &tex);
  static void DrawLine(const Vec2 &p1, const Vec2 &p2, const Vec4 &color);
  static void DrawText(const std::string &text, const Shared<Font> &font, const Vec2 &pos);
  static void Flush();
  static void Shutdown();

private:
  struct QuadVertex {
    f32 pos[3];
    f32 tex_coord[2];
    f32 tex_index;
  };

  struct CircleVertex {
    // TODO:
  };

  struct LineVertex {
    f32 pos[3];
  };
  struct TextVertex {
    f32 position[3];
    f32 tex_coord[2];
  };

  struct Renderer2DData {
    static const u32 max_quads = 1000;
    static const u32 vertices_per_quad = 4;
    static const u32 indices_per_quad = 6;
    static const u32 max_quad_vertices = max_quads * vertices_per_quad;
    static const u32 max_indices = max_quads * indices_per_quad;
    static const u32 max_texture_slots = 32;

    QuadVertex *quad_vertices_base;
    QuadVertex *quad_vertex_current;
    u32 quad_index_count;

    Shared<VertexBuffer> quad_vertex_buffer;
    Shared<VertexArray> quad_vertex_array;
    Shared<Shader> quad_shader;

    Shared<VertexBuffer> circle_vertex_buffer;
    Shared<VertexArray> circle_vertex_array;
    Shared<Shader> circle_shader;

    Shared<VertexBuffer> text_vertex_buffer;
    Shared<VertexArray> text_vertex_array;
    Shared<Shader> text_shader;

    Shared<VertexBuffer> line_vertex_buffer;
    Shared<VertexArray> line_vertex_array;
    f32 line_width = 1.0f;

    Shared<Texture2D> white_texture;

    Shared<Texture2D> texture_slots[max_texture_slots];

    u32 texture_slot_index = 1;
  };

  static Renderer2DData data;

};