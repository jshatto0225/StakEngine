#pragma once

#include <string>
#include <vector>

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

inline u32 size_of(ShaderDataType type) {
  switch (type) {
  case ShaderDataType::FLOAT:
    return 4;
  case ShaderDataType::FLOAT2:
    return 4 * 2;
  case ShaderDataType::FLOAT3:
    return 4 * 3;
  case ShaderDataType::FLOAT4:
    return 4 * 4;
  case ShaderDataType::MAT3:
    return 4 * 3 * 3;
  case ShaderDataType::MAT4:
    return 4 * 4 * 4;
  case ShaderDataType::INT:
    return 4;
  case ShaderDataType::INT2:
    return 4 * 2;
  case ShaderDataType::INT3:
    return 4 * 3;
  case ShaderDataType::INT4:
    return 4 * 4;
  case ShaderDataType::BOOL:
    return 1;
  }
  return 0;
}

struct BufferElement {
  std::string name;
  ShaderDataType type;
  u32 size;
  u64 offset;
  bool normalized;

  BufferElement(ShaderDataType type, const std::string &name, bool normalized) {
    this->name = name;
    this->type = type;
    this->normalized = normalized;
    this->offset = 0;
    this->size = size_of(type);
  }

  u32 get_component_count() const {
    switch (type) {
    case ShaderDataType::FLOAT:
      return 1;
    case ShaderDataType::FLOAT2:
      return 2;
    case ShaderDataType::FLOAT3:
      return 3;
    case ShaderDataType::FLOAT4:
      return 4;
    case ShaderDataType::MAT3:
      return 3;
    case ShaderDataType::MAT4:
      return 4;
    case ShaderDataType::INT:
      return 1;
    case ShaderDataType::INT2:
      return 2;
    case ShaderDataType::INT3:
      return 3;
    case ShaderDataType::INT4:
      return 4;
    case ShaderDataType::BOOL:
      return 1;
    default:
      return 0;
    }
  }
};

class BufferLayout final {
public:
  inline BufferLayout() { stride = 0; }

  BufferLayout(std::initializer_list<BufferElement> elements) {
    this->elements = elements;
    u64 offset = 0;
    stride = 0;
    for (BufferElement &element : this->elements) {
      element.offset = offset;
      offset += element.size;
      stride += element.size;
    }
  }

  inline u32 get_stride() const { return stride; }
  inline const std::vector<BufferElement> get_elements() const {
    return elements;
  }

  std::vector<BufferElement>::iterator begin() { return elements.begin(); }
  std::vector<BufferElement>::iterator end() { return elements.end(); }
  std::vector<BufferElement>::const_iterator begin() const {
    return elements.begin();
  }
  std::vector<BufferElement>::const_iterator end() const {
    return elements.end();
  }

private:
  std::vector<BufferElement> elements;
  u32 stride;
};

class IndexBuffer {
public:
  IndexBuffer() = default;
  virtual ~IndexBuffer(){};

  virtual void bind() const = 0;
  virtual void unbind() const = 0;

  virtual u32 get_count() const = 0;
};

class VertexBuffer {
public:
  VertexBuffer() = default;
  virtual ~VertexBuffer() {}

  virtual void bind() const = 0;
  virtual void unbind() const = 0;
  virtual void set_data(const void *data, u32 size) = 0;

  virtual const BufferLayout &get_layout() const = 0;
  virtual void set_layout(const BufferLayout &layout) = 0;
};

