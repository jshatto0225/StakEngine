#include "Camera.h"

namespace sk {
std::array<std::array<f32, 4>, 4> Camera::get_view_projection() const {
  return {};
}

std::array<std::array<f32, 4>, 4> Camera::get_projection() const { return {}; }
} // namespace SK
