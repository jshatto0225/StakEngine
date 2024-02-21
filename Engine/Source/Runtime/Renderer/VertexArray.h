#pragma once

#include "Buffer.h"

class VertexArray {
public:
  VertexArray() = default;
  virtual ~VertexArray() {}

  virtual void add_vertex_buffer(VertexBuffer *vbo) = 0;
  virtual void set_index_buffer(IndexBuffer *ibo) = 0;
  virtual void bind() const = 0;
  virtual void unbind() const = 0;

  virtual const std::vector<VertexBuffer *> &get_vertex_buffers() const = 0;
  virtual const IndexBuffer *get_index_buffer() const = 0;
};

