#pragma once

#include <array>

#include "Types.h"

namespace SK
{
    class Camera
    {
    public:
        std::array<std::array<f32, 4>, 4> GetViewProjection() const;
        std::array<std::array<f32, 4>, 4> GetProjection() const;
    };
}

