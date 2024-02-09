#pragma once

#include "Texture.h"

namespace sk {
class OpenGLTexture : public Texture {
public:
  OpenGLTexture(const TextureSpecification &textureSpecification);
  ~OpenGLTexture();

  const TextureSpecification &get_specification() const override;
  u32 get_width() const override;
  u32 get_height() const override;
  u32 get_renderer_id() const override;
  const std::string &get_path() const override;
  void set_data(void *data, u32 size) override;
  void bind(u32 slot = 0) const override;
  bool is_loaded() const override;
  bool operator==(const Texture &other) const override;

private:
  TextureSpecification spec;
  std::string path;
  u32 renderer_id;
};

class OpenGLTexture2D : public Texture2D {
public:
  OpenGLTexture2D(const TextureSpecification &textureSpecification);
  ~OpenGLTexture2D();

  const TextureSpecification &get_specification() const override;
  u32 get_width() const override;
  u32 get_height() const override;
  u32 get_renderer_id() const override;
  const std::string &get_path() const override;
  void set_data(void *data, u32 size) override;
  void bind(u32 slot = 0) const override;
  bool is_loaded() const override;
  bool operator==(const Texture &other) const override;

private:
  TextureSpecification spec;
  std::string path;
  u32 renderer_id;
};
} // namespace SK
