#include "OpenGLBuffer.h"

#include "OpenGLBindings.h"

OpenGLIndexBuffer::OpenGLIndexBuffer(u32 *indices, u32 size) {
  count = size;
  glGenBuffers(1, &renderer_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(u32), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
  glDeleteBuffers(1, &renderer_id);
}

void OpenGLIndexBuffer::bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
}

void OpenGLIndexBuffer::unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 OpenGLIndexBuffer::get_count() const {
  return count;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(u32 size) {
  glGenBuffers(1, &renderer_id);
  glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() { glDeleteBuffers(1, &renderer_id); }

void OpenGLVertexBuffer::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
}

void OpenGLVertexBuffer::unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::set_data(const void *data, u32 size) {
  glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

const BufferLayout &OpenGLVertexBuffer::get_layout() const {
  return layout;
}

void OpenGLVertexBuffer::set_layout(const BufferLayout &layout) {
  this->layout = layout;
}

