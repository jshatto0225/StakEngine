#pragma once

#include <string>
#include <vector>

#include "Types.h"
#include "StakMath.h"
#include "Font.h"
#include "Camera.h"

namespace sk {
/**
 * Data types for GLSL shaders
 */
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

/**
 * Size of shader data types
 */
u32 ShaderDataTypeSize(ShaderDataType type);

/**
 * One element of a buffer
 * Contains name, type, size, offset, normalized
 */
class BufferElement {
public:
  /**
   * Create element from type, name, and normalized
   */
  BufferElement(ShaderDataType type, const std::string &name, bool normalized);

  /**
   * Get number of components in element
   */
  u32 getComponentCount() const;

  /**
   * Getters
   */
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

/**
 * Class to store a buffer layout
 */
class BufferLayout {
public:
  /**
   * Empty buffer layout
   */
  BufferLayout();

  /**
   * Create layout from a list of elements
   */
  BufferLayout(std::initializer_list<BufferElement> elements);

  /**
   * Getters
   */
  u32 getStride() const;
  const std::vector<BufferElement> getElements() const;

  /**
   * Iterator access
   */
  std::vector<BufferElement>::iterator begin();
  std::vector<BufferElement>::iterator end();
  std::vector<BufferElement>::const_iterator begin() const;
  std::vector<BufferElement>::const_iterator end() const;

private:
  std::vector<BufferElement> elements;
  u32 stride;
};

/**
 * Vertex Buffer Class
 */
class VertexBuffer {
public:
  /**
   * Create buffer with size
   */
  VertexBuffer(u32 size);
  /**
   * Create buffer with size and vertices
   */
  VertexBuffer(float *vertices, u32 size);
  ~VertexBuffer();

  /**
   * Bind/Unbind buffer
   */
  void bind() const;
  void unbind() const;

  /**
   * Set buffer data
   */
  void setData(const void *data, u32 size);

  /**
   * Get/Set buffer layout
   */
  const BufferLayout &getLayout() const;
  void setLayout(const BufferLayout &layout);

private:
  BufferLayout layout;
  u32 renderer_id;
};

/**
 * Index Buffer Class
 */
class IndexBuffer {
public:
  /**
   * Create empty buffer
   */
  IndexBuffer();

  /**
   * Create buffer from indices and count (num indices)
   */
  IndexBuffer(u32 *indices, u32 count);
  ~IndexBuffer();

  /**
   * Bind/Unbind
   */
  void bind() const;
  void unbind() const;

  /**
   * Get num indices
   */
  u32 getCount() const;

private:
  u32 count;
  u32 renderer_id;
};

/**
 * Vertex Array class
 */
class VertexArray {
public:
  /**
   * Allocate space for the array
   */
  VertexArray();
  ~VertexArray();

  /**
   * Add a vertex buffer
   */
  void addVertexBuffer(Ref<VertexBuffer> vbo);

  /**
   * Set an index buffer
   */
  void setIndexBuffer(Ref<IndexBuffer> ibo);

  /**
   * Bind/Unbind
   */
  void bind() const;
  void unbind() const;

  /**
   * Getters
   */
  const std::vector<Ref<VertexBuffer>> &getVertexBuffers() const;
  const Ref<IndexBuffer> getIndexBuffer() const;

private:
  std::vector<Ref<VertexBuffer>> vertex_buffers;
  Ref<IndexBuffer> index_buffer;
  u32 renderer_id;
  u32 vertex_buffer_index;
};

/**
 * Shader Source Code Struct
 */
struct ShaderSource {
  std::string vs;
  std::string fs;
};

/**
 * Shader Type
 */
enum class ShaderType {
  FRAGMENT,
  VERTEX,
  NONE,
};

/**
 * Sahder Class
 */
class Shader {
public:
  /**
   * Create Shader from source strings
   */
  Shader(const std::string &vs, const std::string &fs);

  /**
   * Create shader from file
   */
  Shader(const std::string &path);
  ~Shader();

  /**
   * Parse and split file into two source strings
   */
  ShaderSource Parse(const std::string &path);

  /**
   * Compile shader
   */
  void compile(const std::string &vs, const std::string &fs);

  /**
   * Bind/Unbind
   */
  void bind() const;
  void unbind() const;

private:
  u32 renderer_id;
};

/**
 * Image formats
 */
enum class ImageFormat {
  R8,
  RGB8,
  RGBA8,
  RGBA32F
};

/**
 * Specification for creating a texture
 */
struct TextureSpecification {
  i32 width = 1;
  i32 height = 1;
  ImageFormat format = ImageFormat::RGBA8;
  bool generate_mips = false;
};

/**
 * Texture class
 * TODO: Incomplete
 */
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

/**
 * 2D Texture Class
 */
class Texture2D {
public:
  /**
   * Create Texture from file
   */
  Texture2D(const std::string &path);

  /**
   * Create texture from specification
   */
  Texture2D(const TextureSpecification &textureSpecification);
  ~Texture2D();

  /**
   * Getters
   */
  const TextureSpecification &getSpecification() const;
  u32 getWidth() const;
  u32 getHeight() const;
  u32 getRendererId() const;
  const std::string &getPath() const;

  /**
   * Set texture data manually
   */
  void setData(void *data, u32 size);

  /**
   * Bind texture to slot
   */
  void bind(u32 slot = 0) const;

  /**
   * Is texture loaded (does it have data)
   */
  bool isLoaded() const;

  /**
   * Comparison
   */
  bool operator==(const Texture2D &other) const;

private:
  TextureSpecification spec;
  std::string path;
  u32 renderer_id;
  bool loaded;
};

/**
* Uniform buffer
*/
class UniformBuffer {
public:
  /**
   * Create buffer form size and binding
   */
  UniformBuffer(u32 size, u32 binding);
  ~UniformBuffer();

  /**
   * Set data
   */
  void setData(const void *data, u32 size, u32 offset = 0);

private:
  u32 m_renderer_id;
};

/**
 * Renderer API
 * Interacts with underlying renderer api (opengl, vilkan, etc)
 */
class RenderApi {
public:
  static void Init();
  static void Shutdown();

  /**
   * Set background clear color
   */
  static void SetClearColor(f32 r, f32 g, f32 b, f32 a);

  /**
   * Set api viewport
   *
   * NOTE: x and y are relative to the window. (0, 0) is
   * always the bottom left
   */
  static void SetViewport(i32 x, i32 y, i32 width, i32 height);

  /**
   * Set line width
   */
  static void SetLineWidth(f32 width);

  /**
   * Bind API functions
   */
  static void Bind();

  /**
   * Clear screen
   */
  static void Clear();

  /**
   * Draw count elements from vao
   */
 static void DrawIndexed(Ref<VertexArray> vao, u32 count);

 /**
  * Draw cout lines from vao
  */
 static void DrawLines(Ref<VertexArray> vao, u32 count);

private:
  static bool initialized;
};

/**
 * Render Commands
 * How an application can interact with the renderer
 */
class RenderCommand {
public:
  static void Init();
  static void Shutdown();

  /**
   * Clear Color
   */
  static void SetClearColor(f32 r, f32 g, f32 b, f32 a);

  /**
   * viewport
   */
  static void SetViewport(i32 x, i32 y, i32 width, i32 height);

  /**
   * Line Width
   */
  static void SetLineWidth(f32 width);

  /**
   * Bind
   */
  static void Bind();

  /**
   * Clear
   */
  static void Clear();

  /**
   * Draws
   */
  static void DrawIndexed(Ref<VertexArray> vao, u32 count);
  static void DrawLines(Ref<VertexArray> vao, u32 count);
};

/**
 * Main Renderer Class
 *
 * High level general renderer
 */
class Renderer {
public:
  static void Init();
  static void Shutdown();

  /**
   * Window Resize Event Function
   */
  static void OnWindowResize(i32 width, i32 height);

  /**
   * Start Scene
   */
  static void BeginScene();

  /**
   * End Scene
   */
  static void EndScene();

  /**
   * Submit shader and vao
   */
  static void Submit(Ref<Shader> shader, Ref<VertexArray> vao);
};

/**
 * 2D Renderer
 */
class Renderer2D {
public:
  static void Init();

  /**
   * Start Scene With Camera
   */
  static void BeginScene(const SceneViewCamera &cam);

  /**
   * End Scene
   * Draws all buffered objects
   */
  static void EndScene();
  static void Shutdown();

  /**
   * Draw Quad
   */
  static void DrawQuad(const Vec2 &pos, const Vec2 &size, const Vec4 &color);
  static void DrawQuad(const Vec2 &pos, const Vec2 &size, const Ref<Texture2D> &tex);

  /**
   * Draw a quad that is rotated rotation radians
   */
  static void DrawQuad(const Vec2 &pos, const Vec2 &size, f32 rotation, const Vec4 &color);
  static void DrawQuad(const Vec2 &pos, const Vec2 &size, f32 rotation, const Ref<Texture2D> &tex);
  static void DrawQuad(const Mat4 &transform, const Ref<Texture2D> &tex);

  /**
   * Draw Circle
   */
  static void DrawCircle(const Vec2 &pos, f32 radius, const Vec4 &color);
  static void DrawCircle(const Vec2 &pos, f32 radius, const Ref<Texture2D> &tex);

  /**
   * Draw Line
   */
  static void DrawLine(const Vec2 &p1, const Vec2 &p2, const Vec4 &color);

  /**
   * Draw Text
   */
  static void DrawText(const std::string &text, const Ref<Font> &font, const Vec2 &pos);

private:
  /**
   * Start batch for batch rendering
   */
  static void StartBatch();

  /**
   * Flush current batch and advance to next batch
   */
  static void NextBatch();

  /**
   * Draw elements in batch
   */
  static void Flush();

private:
  struct QuadVertex {
    Vec3 pos;
    Vec2 tex_coord;
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
    static constexpr u32 max_quads = 1000;
    static constexpr u32 vertices_per_quad = 4;
    static constexpr u32 indices_per_quad = 6;
    static constexpr u32 max_quad_vertices = max_quads * vertices_per_quad;
    static constexpr u32 max_indices = max_quads * indices_per_quad;
    static constexpr u32 max_texture_slots = 32;

    QuadVertex *quad_vertices_base;
    QuadVertex *quad_vertex_current;
    u32 quad_index_count;

    Ref<VertexBuffer> quad_vertex_buffer;
    Ref<VertexArray> quad_vertex_array;
    Ref<Shader> quad_shader;

    Ref<VertexBuffer> circle_vertex_buffer;
    Ref<VertexArray> circle_vertex_array;
    Ref<Shader> circle_shader;

    Ref<VertexBuffer> text_vertex_buffer;
    Ref<VertexArray> text_vertex_array;
    Ref<Shader> text_shader;

    Ref<VertexBuffer> line_vertex_buffer;
    Ref<VertexArray> line_vertex_array;
    f32 line_width = 1.0f;

    Ref<Texture2D> white_texture;

    Ref<Texture2D> texture_slots[max_texture_slots];

    u32 texture_slot_index = 1;

    struct CameraData {
      Mat4 view_proj;
    };
    CameraData cam_data;
    Ref<UniformBuffer> cam_uniform_buffer;

    Vec3 quad_vertex_positions[4];
    Vec2 quad_tex_coords[4];
  };

  static Renderer2DData data;
};

} // namespace sk
