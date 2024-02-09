#include "OpenGLTexture.h"
#include "Texture.h"

namespace sk {
OpenGLTexture::OpenGLTexture(const TextureSpecification &textureSpecification) {

}

OpenGLTexture::~OpenGLTexture() {}

const TextureSpecification &OpenGLTexture::get_specification() const {
  return spec;
}

u32 OpenGLTexture::get_width() const {
  return spec.width;
}

u32 OpenGLTexture::get_height() const {
  return spec.height;
}

u32 OpenGLTexture::get_renderer_id() const {
  return renderer_id;
}

const std::string &OpenGLTexture::get_path() const {
  return path;
}

void OpenGLTexture::set_data(void *data, u32 size) {}

void OpenGLTexture::bind(u32 slot) const {}

bool OpenGLTexture::is_loaded() const { return false; }

bool OpenGLTexture::operator==(const Texture &other) const {
  return renderer_id == other.get_renderer_id();
}

OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification &textureSpecification) {}

OpenGLTexture2D::~OpenGLTexture2D() {}

const TextureSpecification &OpenGLTexture2D::get_specification() const {
  return spec;
}

u32 OpenGLTexture2D::get_width() const {
  return spec.width;
}

u32 OpenGLTexture2D::get_height() const {
  return spec.height;
}

u32 OpenGLTexture2D::get_renderer_id() const {
  return renderer_id;
}

const std::string &OpenGLTexture2D::get_path() const {
  return path;
}

void OpenGLTexture2D::set_data(void *data, u32 size) {}

void OpenGLTexture2D::bind(u32 slot) const {}

bool OpenGLTexture2D::is_loaded() const { return false; }

bool OpenGLTexture2D::operator==(const Texture &other) const {
  return renderer_id == other.get_renderer_id();
}
} // namespace SK
