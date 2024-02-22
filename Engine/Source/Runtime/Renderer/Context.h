#pragma once

#include "Platform.h"

struct SKWindow;
struct _SKWindow;
struct _SKContext;

void sk_make_context_current(SKWindow *);
void sk_init_context(_SKWindow *);
