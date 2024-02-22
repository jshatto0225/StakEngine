#pragma once

#include <X11/Xlib.h>
#include <GL/glx.h>

struct _X11Window {
  Window window;
  Window root;
  XSetWindowAttributes attribs;
  XVisualInfo *visual_info;
  Colormap colormap;
};

struct _X11Platform {
  Display *display;
  Atom wm_delete_window;
};

struct _X11Context {
  GLXContext gl_context;
};

#define SK_X11_CONTEXT_STATE _X11Context x11ctx;
#define SK_X11_PLATFORM_STATE _X11Platform x11;
#define SK_X11_WINDOW_STATE _X11Window x11;