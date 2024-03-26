#pragma once

#include <windows.h>
#undef CreateWindow
#undef DestroyWindow

#include "Types.h"
#include "Event.h"

struct platform {
  HINSTANCE Instance;
  ATOM DefaultWindowClass;
  HWND DummyWindow;
  bool Initialized;
  HINSTANCE RendererApi;
};

struct window {
  HWND Handle;
  i32 X;
  i32 Y;
  i32 Width;
  i32 Height;
  const char *Title;
  void(*EventFn)(void *, const event *);
  context *Context;
  void *Parent;
};

extern platform Platform;

