#pragma once

#include <X11/Xlib.h>
#include <GL/glx.h>
using XWindow = Window;

namespace sk {
struct X11Window {
  XWindow window;
  XWindow root;
  XSetWindowAttributes attribs;
  XVisualInfo *visual_info;
  Colormap colormap;
};

struct X11Platform {
  Display *display;
  Atom wm_delete_window;
};

struct X11Context {
  GLXContext context;
};

} // namespace sk

#define X11_CONTEXT_STATE X11Context x11;
#define X11_PLATFORM_STATE static X11Platform x11;
#define X11_WINDOW_STATE X11Window x11;
