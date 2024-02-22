#pragma once

#include "Buffer.h"

class OpenGLIndexBuffer : public IndexBuffer {
public:
  OpenGLIndexBuffer(u32 *indices, u32 size);
  ~OpenGLIndexBuffer();

  void bind() const override;
  void unbind() const override;

  u32 get_count() const override;

private:
  u32 renderer_id;
  u32 count;
};

class OpenGLVertexBuffer : public VertexBuffer {
public:
  OpenGLVertexBuffer(u32 size);
  ~OpenGLVertexBuffer();

  void bind() const override;
  void unbind() const override;
  void set_data(const void *data, u32 size) override;

  const BufferLayout &get_layout() const override;
  void set_layout(const BufferLayout &layout) override;

private:
  u32 renderer_id;
  BufferLayout layout;
};

