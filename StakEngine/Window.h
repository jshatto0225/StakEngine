#pragma once

#include "Types.h"
#include "StakMath.h"
#include "Event.h"

struct window_config {
  i32 X;
  i32 Y;
  i32 Width;
  i32 Height;
  const char *Title;
  void *Parent;
};

struct window_size_data {
  i32 Width;
  i32 Height;
};

struct window_pos_data {
  i32 X;
  i32 Y;
};

struct window;
struct context;

context *CreateContext(const window *Window);
void DestroyContext(context **Context);
void MakeContextCurrent(const context *Context);
void SwapContextBuffers(const context *Context);

window *CreateWindow(const window_config *Config);
void DestroyWindow(window **Window);
void SwapWindowBuffers(window *Window);
void SetWindowPos(window *window, i32 X, i32 Y);
void SetWindowSize(window *Window, i32 Width, i32 Height);
window_pos_data GetWindowPos(const window *Window);
window_size_data GetWindowSize(const window *Window);
void SetWindowEventFn(window *Window, void(*EventFn)(void *, const event *));
void UpdateWindow(const window *window);
void MakeWindowCurrent(const window *Window);
