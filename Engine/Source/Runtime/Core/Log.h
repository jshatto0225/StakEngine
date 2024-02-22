#pragma once

#include <stdio.h>

void sk_debug_core_trace(const char *fmt, ...);
void sk_debug_core_info(const char *fmt, ...);
void sk_debug_core_warn(const char *fmt, ...);
void sk_debug_core_error(const char *fmt, ...);
void sk_debug_core_critical(const char *fmt, ...);

void sk_debug_trace(const char *fmt, ...);
void sk_debug_info(const char *fmt, ...);
void sk_debug_warn(const char *fmt, ...);
void sk_debug_error(const char *fmt, ...);
void sk_debug_critical(const char *fmt, ...);
