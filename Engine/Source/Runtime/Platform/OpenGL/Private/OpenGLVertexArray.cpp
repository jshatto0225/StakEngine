#include "OpenGLVertexArray.h"

#include "Log.h"
#include "OpenGLBindings.h"

namespace sk {
static GLenum shader_data_type_to_opengl_type(ShaderDataType type) {
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
    SK_CORE_CRITICAL("Unknown Data Type");
    return 0;
  }
}

OpenGLVertexArray::OpenGLVertexArray() {
  glCreateVertexArrays(1, &renderer_id);
  vertex_buffer_index = 0;
}

OpenGLVertexArray::~OpenGLVertexArray() {
  glDeleteVertexArrays(1, &renderer_id);
}

// TODO: This is a mess lol
void OpenGLVertexArray::add_vertex_buffer(const Shared<VertexBuffer> vbo) {
  bind();
  vbo->bind();
  const BufferLayout &layout = vbo->get_layout();
  for (const BufferElement &element : layout) {
    switch (element.type) {
    case ShaderDataType::FLOAT4: {
      glEnableVertexAttribArray(vertex_buffer_index);
      glVertexAttribPointer(vertex_buffer_index, element.get_component_count(),
                            shader_data_type_to_opengl_type(element.type),
                            element.normalized ? GL_TRUE : GL_FALSE,
                            layout.get_stride(), (const void *)element.offset);
      vertex_buffer_index++;
      break;
    }
    case ShaderDataType::BOOL: {
      glEnableVertexAttribArray(vertex_buffer_index);
      glVertexAttribPointer(vertex_buffer_index, element.get_component_count(),
                            shader_data_type_to_opengl_type(element.type),
                            element.normalized ? GL_TRUE : GL_FALSE,
                            layout.get_stride(), (const void *)element.offset);
      vertex_buffer_index++;
      break;
    }
    case ShaderDataType::MAT4: {
      u8 count = element.get_component_count();
      for (u8 i = 0; i < count; i++) {
        glEnableVertexAttribArray(vertex_buffer_index);
        glVertexAttribPointer(
            vertex_buffer_index, count, size_of(element.type),
            element.normalized ? GL_TRUE : GL_FALSE, layout.get_stride(),
            (const void *)(element.offset + sizeof(f32) * count * i));
        glVertexAttribDivisor(vertex_buffer_index, 1);
        vertex_buffer_index++;
      }
    }
    }
  }
}

void OpenGLVertexArray::set_index_buffer(const Shared<IndexBuffer> ibo) {
  bind();
  ibo->bind();
  index_buffer = ibo;
}

void OpenGLVertexArray::bind() const {
  glBindVertexArray(renderer_id);
}

void OpenGLVertexArray::unbind() const {
  glBindVertexArray(0);
}

const std::vector<Shared<VertexBuffer>> &OpenGLVertexArray::get_vertex_buffers() const {
  return vertex_buffers;
}

const Shared<IndexBuffer> OpenGLVertexArray::get_index_buffer() const {
  return index_buffer;
}
} // namespace SK
