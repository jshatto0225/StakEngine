#pragma once

#include "Buffer.h"

namespace sk {
class VertexArray {
public:
  VertexArray() = default;
  virtual ~VertexArray() {}

  virtual void add_vertex_buffer(const Shared<VertexBuffer> vbo) = 0;
  virtual void set_index_buffer(const Shared<IndexBuffer> ibo) = 0;
  virtual void bind() const = 0;
  virtual void unbind() const = 0;

  virtual const std::vector<Shared<VertexBuffer>> &get_vertex_buffers() const = 0;
  virtual const Shared<IndexBuffer> get_index_buffer() const = 0;
};
} // namespace SK
