#pragma once

#include "VertexArray.h"

class OpenGLVertexArray : public VertexArray {
public:
  OpenGLVertexArray();
  ~OpenGLVertexArray();

  void add_vertex_buffer(VertexBuffer *vbo) override;
  void set_index_buffer(IndexBuffer *ibo) override;
  void bind() const override;
  void unbind() const override;

  const std::vector<VertexBuffer *> &get_vertex_buffers() const override;
  const IndexBuffer *get_index_buffer() const override;

private:
  std::vector<VertexBuffer *> vertex_buffers;
  IndexBuffer *index_buffer;
  u32 renderer_id;
  u32 vertex_buffer_index;
};

