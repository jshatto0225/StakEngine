#pragma once

#include "Types.h"

struct SKWindow;
struct SKWindow;
struct _SKWindow;
struct _SKContext;

// Render Commands
void sk_render_command_set_clear_color(f32, f32, f32, f32);
void sk_render_command_clear();
void sk_render_command_init(SKWindow *);
void sk_render_command_set_viewport(SKWindow *);
void sk_render_command_shutdown();

// Rendering Context Stuff
void sk_make_context_current(SKWindow *);
void _sk_create_context(_SKWindow *);
void sk_bind_renderer();

