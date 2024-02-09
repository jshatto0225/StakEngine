#pragma once

#include "VertexArray.h"

namespace sk {
class OpenGLVertexArray : public VertexArray {
public:
  OpenGLVertexArray();
  ~OpenGLVertexArray();

  void add_vertex_buffer(const Shared<VertexBuffer> vbo) override;
  void set_index_buffer(const Shared<IndexBuffer> ibo) override;
  void bind() const override;
  void unbind() const override;

  const std::vector<Shared<VertexBuffer>> &get_vertex_buffers() const override;
  const Shared<IndexBuffer> get_index_buffer() const override;

private:
  std::vector<Shared<VertexBuffer>> vertex_buffers;
  Shared<IndexBuffer> index_buffer;
  u32 renderer_id;
  u32 vertex_buffer_index;
};
} // namespace SK
