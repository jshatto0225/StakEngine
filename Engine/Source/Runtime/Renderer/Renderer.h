#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Types.h"

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

  void addVertexBuffer(std::shared_ptr<VertexBuffer> vbo);
  void setIndexBuffer(std::shared_ptr<IndexBuffer> ibo);
  void bind() const;
  void unbind() const;

  const std::vector<std::shared_ptr<VertexBuffer>> &getVertexBuffers() const;
  const std::shared_ptr<IndexBuffer> getIndexBuffer() const;

private:
  std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers;
  std::shared_ptr<IndexBuffer> index_buffer;
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
  u32 width = 1;
  u32 height = 1;
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

 static void DrawIndexed(std::shared_ptr<VertexArray> vao, u32 count);
 static void DrawLines(std::shared_ptr<VertexArray> vao, u32 count);

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
  static void DrawIndexed(std::shared_ptr<VertexArray> vao, u32 count);
  static void DrawLines(std::shared_ptr<VertexArray> vao, u32 count);
};

class Renderer {
public:
  static void Init();
  static void Shutdown();
  static void OnWindowResize(i32 width, i32 height);
  static void BeginScene();
  static void EndScene();
  static void Submit(std::shared_ptr<Shader> shader, std::shared_ptr<VertexArray> vao);
};