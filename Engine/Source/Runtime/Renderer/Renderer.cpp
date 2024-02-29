#include "Renderer.h"

u32 ShaderDataTypeSize(ShaderDataType type) {
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

BufferElement::BufferElement(ShaderDataType type, const std::string &name, bool normalized) {
  this->name = name.c_str();
  this->type = type;
  this->normalized = normalized;
  this->offset = 0;
  this->size = ShaderDataTypeSize(type);
}

u32 BufferElement::getComponentCount() const {
  switch (this->type) {
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
}

const char *BufferElement::getName() const {
  return this->name;
}

ShaderDataType BufferElement::getType() const {
  return this->type;
}

u32 BufferElement::getSize() const {
  return this->size;
}

u64 BufferElement::getOffset() const {
  return this->offset;
}

bool BufferElement::isNormalized() const {
  return this->normalized;
}

BufferLayout::BufferLayout() {
  this->stride = 0;
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements) {
  this->elements = elements;
  u64 offset = 0;
  this->stride = 0;
  for (BufferElement &element : this->elements) {
    element.offset = offset;
    offset += element.getSize();
    stride += element.getSize();
  }
}

u32 BufferLayout::getStride() const {
  return this->stride;
}

const std::vector<BufferElement> BufferLayout::getElements() const {
  return this->elements;
}

std::vector<BufferElement>::iterator BufferLayout::begin() {
  return elements.begin();
}

std::vector<BufferElement>::iterator BufferLayout::end() {
  return elements.end();
}

std::vector<BufferElement>::const_iterator BufferLayout::begin() const {
  return elements.begin();
}

std::vector<BufferElement>::const_iterator BufferLayout::end() const {
  return elements.end();
}
