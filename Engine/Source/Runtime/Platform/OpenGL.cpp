#include "OpenGL.h"

#include <fstream>

#include "Platform.h"
#include "Renderer.h"
#include "Window.h"
#include "Log.h"

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

void RenderApi::Bind() {
  const unsigned char *version = glGetString(GL_VERSION);
  Log::CoreTrace("%s", version);

  glGenBuffers = (PFNGLGENBUFFERSPROC)Platform::GetProcAddress("glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERPROC)Platform::GetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)Platform::GetProcAddress("glBufferData");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)Platform::GetProcAddress("glDeleteBuffers");
  glBufferSubData = (PFNGLBUFFERSUBDATAPROC)Platform::GetProcAddress("glBufferSubData");
  glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)Platform::GetProcAddress("glDebugMessageCallback");
  glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)Platform::GetProcAddress("glDebugMessageControl");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)Platform::GetProcAddress("glGenVertexArrays");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)Platform::GetProcAddress("glDeleteVertexArrays");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)Platform::GetProcAddress("glEnableVertexAttribArray");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)Platform::GetProcAddress("glVertexAttribPointer");
  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)Platform::GetProcAddress("glVertexAttribDivisor");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)Platform::GetProcAddress("glBindVertexArray");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)Platform::GetProcAddress("glCreateProgram");
  glCreateShader = (PFNGLCREATESHADERPROC)Platform::GetProcAddress("glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)Platform::GetProcAddress("glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC)Platform::GetProcAddress("glCompileShader");
  glAttachShader = (PFNGLATTACHSHADERPROC)Platform::GetProcAddress("glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)Platform::GetProcAddress("glLinkProgram");
  glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)Platform::GetProcAddress("glValidateProgram");
  glDeleteShader = (PFNGLDELETESHADERPROC)Platform::GetProcAddress("glDeleteShader");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC)Platform::GetProcAddress("glDeleteProgram");
  glUseProgram = (PFNGLUSEPROGRAMPROC)Platform::GetProcAddress("glUseProgram");
  glGetShaderiv = (PFNGLGETSHADERIVPROC)Platform::GetProcAddress("glGetShaderiv");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)Platform::GetProcAddress("glGetProgramiv");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)Platform::GetProcAddress("glGetShaderInfoLog");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)Platform::GetProcAddress("glGetProgramInfoLog");
}

static GLenum ShaderDataTypeToOpenglType(ShaderDataType type) {
  switch (type) {
  case ShaderDataType::FLOAT:
    return GL_FLOAT;
  case ShaderDataType::FLOAT2:
    return GL_FLOAT;
  case ShaderDataType::FLOAT3:
    return GL_FLOAT;
  case ShaderDataType::FLOAT4:
    return GL_FLOAT;
  case ShaderDataType::MAT3:
    return GL_FLOAT;
  case ShaderDataType::MAT4:
    return GL_FLOAT;
  case ShaderDataType::INT:
    return GL_INT;
  case ShaderDataType::INT2:
    return GL_INT;
  case ShaderDataType::INT3:
    return GL_INT;
  case ShaderDataType::INT4:
    return GL_INT;
  case ShaderDataType::BOOL:
    return GL_BOOL;
  default:
    Log::CoreCritical("Unknown Data Type");
    return 0;
  }
}

static GLenum ImageFormatToOpenGLDataFormat(ImageFormat format) {
  switch (format) {
  case ImageFormat::R8:
    return GL_R8;
  case ImageFormat::RGB8:
    return GL_RGB8;
  case ImageFormat::RGBA8:
    return GL_RGBA8;
  case ImageFormat::RGBA32F:
    return GL_RGBA32F;
  }
}

static void APIENTRY OpenGLMessageCallback(u32 source, 
                                           u32 type, 
                                           u32 id, u32 
                                           severity, 
                                           i32 length, 
                                           const char *message, 
                                           const void *user_param) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    Log::CoreCritical(message);
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    Log::CoreError(message);
    break;
  case GL_DEBUG_SEVERITY_LOW:
    Log::CoreWarn(message);
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    Log::CoreTrace(message);
    break;
  }
}

bool RenderApi::initialized = false;

void RenderApi::Init() {
  if (initialized) {
    return;
  }

#if defined(SK_DEBUG) or defined(SK_RELEASE)
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(OpenGLMessageCallback, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, 
                        GL_DEBUG_SEVERITY_NOTIFICATION, 
                        0, 
                        nullptr, 
                        GL_FALSE);
#endif

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);

  initialized = true;
}

void RenderApi::Shutdown() {
  initialized = false;
}

void RenderApi::SetClearColor(f32 r, f32 g, f32 b, f32 a) {
  glClearColor(r, g, b, a);
}

void RenderApi::Clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderApi::SetViewport(i32 x, i32 y, i32 width, i32 height) {
  glViewport(x, y, width, height);
}

void RenderApi::SetLineWidth(f32 width) {

}

void RenderApi::DrawIndexed(std::shared_ptr<VertexArray> vao, u32 count) {
  if (count == 0) {
    count = vao->getIndexBuffer()->getCount();
  }
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void RenderApi::DrawLines(std::shared_ptr<VertexArray> vao, u32 count) {

}

VertexBuffer::VertexBuffer(u32 size) {
  glGenBuffers(1, &this->renderer_id);
  glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(float *vertices, u32 size) {
  glGenBuffers(1, &this->renderer_id);
  glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &this->renderer_id);
}

void VertexBuffer::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
}

void VertexBuffer::unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setData(const void *data, u32 size) {
  glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void VertexBuffer::setLayout(const BufferLayout &layout) {
  this->layout = layout;
}

const BufferLayout &VertexBuffer::getLayout() const {
  return this->layout;
}

IndexBuffer::IndexBuffer() {
  this->count = 0;

  glGenBuffers(1, &this->renderer_id);
}

IndexBuffer::IndexBuffer(u32 *indices, u32 count) {
  this->count = count;

  glGenBuffers(1, &this->renderer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->renderer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
  glDeleteBuffers(1, &this->renderer_id);
}

void IndexBuffer::bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->renderer_id);
}

void IndexBuffer::unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 IndexBuffer::getCount() const {
  return this->count;
}

VertexArray::VertexArray() {
  glGenVertexArrays(1, &this->renderer_id);
  this->vertex_buffer_index = 0;
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &this->renderer_id);
}

void VertexArray::addVertexBuffer(std::shared_ptr<VertexBuffer> vbo) {
  this->bind();
  vbo->bind();
  const BufferLayout &layout = vbo->getLayout();
  for (const BufferElement &element : layout) {
    switch (element.getType()) {
    case ShaderDataType::INT:
    case ShaderDataType::INT2:
    case ShaderDataType::INT3:
    case ShaderDataType::INT4:
    case ShaderDataType::FLOAT:
    case ShaderDataType::FLOAT2:
    case ShaderDataType::FLOAT3:
    case ShaderDataType::FLOAT4: {
      glEnableVertexAttribArray(this->vertex_buffer_index);
      glVertexAttribPointer(this->vertex_buffer_index, 
                            element.getComponentCount(),
                            ShaderDataTypeToOpenglType(element.getType()),
                            element.isNormalized() ? GL_TRUE : GL_FALSE,
                            layout.getStride(), 
                            (const void *)element.getOffset());
      this->vertex_buffer_index++;
      break;
    }
    case ShaderDataType::BOOL: {
      glEnableVertexAttribArray(this->vertex_buffer_index);
      glVertexAttribPointer(this->vertex_buffer_index, 
                            element.getComponentCount(),
                            ShaderDataTypeToOpenglType(element.getType()),
                            element.isNormalized() ? GL_TRUE : GL_FALSE,
                            layout.getStride(), 
                            (const void *)element.getOffset());
      this->vertex_buffer_index++;
      break;
    }
    case ShaderDataType::MAT3:
    case ShaderDataType::MAT4: {
      u8 count = element.getComponentCount();
      for (u8 i = 0; i < count; i++) {
        glEnableVertexAttribArray(this->vertex_buffer_index);
        glVertexAttribPointer(this->vertex_buffer_index,
                              count,
                              ShaderDataTypeSize(element.getType()),
                              element.isNormalized() ? GL_TRUE : GL_FALSE,
                              layout.getStride(),
                              (const void *)(element.getOffset() + sizeof(f32) * count * i));
        glVertexAttribDivisor(this->vertex_buffer_index, 1);
        this->vertex_buffer_index++;
      }
    }
    }
  }
}

void VertexArray::setIndexBuffer(std::shared_ptr<IndexBuffer> ibo) {
  this->bind();
  ibo->bind();
  this->index_buffer = ibo;
}

void VertexArray::bind() const {
  glBindVertexArray(this->renderer_id);
}

void VertexArray::unbind() const {
  glBindVertexArray(0);
}

const std::vector<std::shared_ptr<VertexBuffer>> &VertexArray::getVertexBuffers() const {
  return this->vertex_buffers;
}

const std::shared_ptr<IndexBuffer> VertexArray::getIndexBuffer() const {
  return this->index_buffer;
}

Shader::Shader(const std::string &vs, const std::string &fs) {
  this->compile(vs, fs);
}

Shader::Shader(const std::string &path) {
  ShaderSource src = Parse(path);
  this->compile(src.vs, src.fs);
}

void Shader::compile(const std::string &vs, const std::string &fs) {
  this->renderer_id = glCreateProgram();
  u32 vs_id = glCreateShader(GL_VERTEX_SHADER);
  const char *src = vs.c_str();
  glShaderSource(vs_id, 1, &src, nullptr);
  glCompileShader(vs_id);
  glAttachShader(this->renderer_id, vs_id);

  u32 fs_id = glCreateShader(GL_FRAGMENT_SHADER);
  src = fs.c_str();
  glShaderSource(fs_id, 1, &src, nullptr);
  glCompileShader(fs_id);
  glAttachShader(this->renderer_id, fs_id);

  glLinkProgram(this->renderer_id);
  glValidateProgram(this->renderer_id);

  i32 result;
  glGetShaderiv(vs_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    Log::CoreError("Failed to compile vertex shader");
  }
  glGetShaderiv(fs_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    Log::CoreError("Failed to compile fragment shader");
  }

  glDeleteShader(vs_id);
  glDeleteShader(fs_id);
}

Shader::~Shader() {
  glDeleteProgram(this->renderer_id);
}

ShaderSource Shader::Parse(const std::string &path) {
  ShaderSource src;
  ShaderType current_type = ShaderType::NONE;
  std::ifstream in(path);
  std::string line;
  while (std::getline(in, line)) {
    switch (current_type) {
    case ShaderType::FRAGMENT:
      if (line.find("#shader vertex") != std::string::npos) {
        current_type = ShaderType::VERTEX;
        break;
      }
      src.fs.append(line);
      src.fs.append("\n");
      break;

    case ShaderType::VERTEX:
      if (line.find("#shader fragment") != std::string::npos) {
        current_type = ShaderType::FRAGMENT;
        break;
      }
      src.vs.append(line);
      src.vs.append("\n");
      break;

    case ShaderType::NONE:
      if (line.find("#shader vertex") != std::string::npos) {
        current_type = ShaderType::VERTEX;
      }
      else if (line == "#shader fragment") {
        current_type = ShaderType::FRAGMENT;
      }
      break;
    }
  }
  return src;
}

void Shader::bind() const {
  glUseProgram(this->renderer_id);
}

void Shader::unbind() const {
  glUseProgram(0);
}

Texture::Texture(const TextureSpecification &textureSpecification) {
  glGenTextures(1, &this->renderer_id);
}

Texture::~Texture() {
  glDeleteTextures(1, &this->renderer_id);
}

const TextureSpecification &Texture::getSpecification() const {
  return this->spec;
}

u32 Texture::getWidth() const {
  return this->spec.width;
}

u32 Texture::getHeight() const {
  return this->spec.height;
}

u32 Texture::getRendererId() const {
  return this->renderer_id;
}

const std::string &Texture::getPath() const {
  return path;
}

void Texture::setData(void *data, u32 size) {

}

void Texture::bind(u32 slot) const {

}

bool Texture::isLoaded() const {
  return false;
}

bool Texture::operator==(const Texture &other) const {
  return this->renderer_id == other.renderer_id;
}

Texture2D::Texture2D(const std::string &path) {
  this->loaded = false;
  this->path = path;

  // LOAD IMAGE

  glGenTextures(1, &this->renderer_id);

  glBindTexture(GL_TEXTURE_2D, this->renderer_id);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture2D::Texture2D(const TextureSpecification &textureSpecification) {
  glGenTextures(1, &this->renderer_id);

  glBindTexture(GL_TEXTURE_2D, this->renderer_id);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  this->loaded = false;
}

Texture2D::~Texture2D() {
  glDeleteTextures(1, &this->renderer_id);
}

const TextureSpecification &Texture2D::getSpecification() const {
  return this->spec;
}

u32 Texture2D::getWidth() const {
  return this->spec.width;
}

u32 Texture2D::getHeight() const {
  return this->spec.height;
}

u32 Texture2D::getRendererId() const {
  return this->renderer_id;
}

const std::string &Texture2D::getPath() const {
  return path;
}

void Texture2D::setData(void *data, u32 size) {
  glTexSubImage2D(this->renderer_id, 
                      0, 
                      0, 
                      0, 
                      this->spec.width, 
                      this->spec.height, 
                      ImageFormatToOpenGLDataFormat(this->spec.format),
                      GL_UNSIGNED_BYTE,
                      data);
  this->loaded = true;
}

void Texture2D::bind(u32 slot) const {
  glBindTexture(GL_TEXTURE_2D, this->renderer_id);
}

bool Texture2D::isLoaded() const {
  return this->loaded;
}

bool Texture2D::operator==(const Texture2D &other) const {
  return this->renderer_id == other.renderer_id;
}
