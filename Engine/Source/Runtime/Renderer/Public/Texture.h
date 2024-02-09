#pragma once

#include <string>

#include "Types.h"

namespace sk {
enum class ImageFormat { R8, RGB8, RGBA8, RGBA32F };

struct TextureSpecification {
  u32 width;
  u32 height;
  ImageFormat format;
  bool generate_mips;

  TextureSpecification() {
    width = 1;
    height = 1;
    format = ImageFormat::RGBA8;
    generate_mips = false;
  }
};

class Texture {
public:
  Texture() = default;
  virtual ~Texture() {}

  virtual const TextureSpecification &get_specification() const = 0;
  virtual u32 get_width() const = 0;
  virtual u32 get_height() const = 0;
  virtual u32 get_renderer_id() const = 0;
  virtual const std::string &get_path() const = 0;
  virtual void set_data(void *data, u32 size) = 0;
  virtual void bind(u32 slot = 0) const = 0;
  virtual bool is_loaded() const = 0;
  virtual bool operator==(const Texture &other) const = 0;
};

class Texture2D : public Texture {
public:
  Texture2D() = default;
  virtual ~Texture2D() {}
};
} // namespace SK
