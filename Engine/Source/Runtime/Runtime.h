#pragma once

#include "Math/Vector/Vec1Types.h"
#include "Math/Vector/Vec2Types.h"
#include "Math/Vector/Vec3Types.h"
#include "Math/Vector/Vec4Types.h"

#include "Math/Matrix/Mat2x2Types.h"
#include "Math/Matrix/Mat3x3Types.h"
#include "Math/Matrix/Mat4x4Types.h"

#include "Core/Types.h"
#include "Core/AppLayer.h"
#include "Core/Event.h"
#include "Core/EntryPoint.h"
#include "Core/Log.h"

struct SKWindow;
struct SKWindowConfig;

SKWindow *sk_create_window(SKWindowConfig *config);
void sk_app_add_layer(SKAppLayer *layer);
