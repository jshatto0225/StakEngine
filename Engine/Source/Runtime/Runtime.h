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

struct SKWindow;
struct SKWindowConfig;

void sk_debug_trace(const char *, ...);
void sk_debug_info(const char *, ...);
void sk_debug_warn(const char *, ...);
void sk_debug_error(const char *, ...);
void sk_debug_critical(const char *, ...);

SKWindow *sk_create_window(SKWindowConfig *config);
void sk_app_add_layer(SKAppLayer *layer);

// They appear in entry point
// Remove outside accessability
#define sk_app_init
#define sk_app_run