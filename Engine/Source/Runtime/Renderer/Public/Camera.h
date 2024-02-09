#pragma once

#include <array>

#include "Types.h"

namespace sk {
class Camera {
public:
  std::array<std::array<f32, 4>, 4> get_view_projection() const;
  std::array<std::array<f32, 4>, 4> get_projection() const;
};
} // namespace SK
