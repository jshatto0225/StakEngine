#if defined(SK_X11)

#include "Platform.h"
#include "Window.h"
#include "Renderer.h"
#include "Event.h"

_SKPlatform _sk;

void sk_platform_init() {
  if (_sk.initialized) {
    return;
  }
  _sk.x11.display = XOpenDisplay(nullptr);
  _sk.x11.wm_delete_window = XInternAtom(_sk.x11.display, "WM_DELETE_WINDOW", False);
}

void sk_platform_shutdown() {
  if (!_sk.initialized) {
    return;
  }
  XCloseDisplay(_sk.x11.display);
}

SKWindow *sk_create_window(const SKWindowConfig &config) {
  _SKWindow *window = new _SKWindow;
  window->x = config.x;
  window->y = config.y;
  window->width = config.width;
  window->height = config.height;
  window->title = config.title;
  window->should_close = false;
  window->x11.root = DefaultRootWindow(_sk.x11.display);

  int attribs[] = {
    GLX_RGBA,
    GLX_DEPTH_SIZE,
    GLX_DOUBLEBUFFER,
    None
  };
  window->x11.visual_info = glXChooseVisual(_sk.x11.display, DefaultScreen(_sk.x11.display), attribs);
  window->x11.colormap = XCreateColormap(_sk.x11.display, window->x11.root, window->x11.visual_info->visual, AllocNone);
  window->x11.attribs.colormap = window->x11.colormap;
  window->x11.attribs.event_mask = ExposureMask | KeyPressMask;

  window->x11.window = XCreateSimpleWindow(_sk.x11.display,
                                           window->x11.root,
                                           window->x,
                                           window->y,
                                           (u32)window->width,
                                           (u32)window->height,
                                           0,
                                           0,
                                           0xffffffff);
  XMapWindow(_sk.x11.display, window->x11.window);
  _sk_create_context(window);

  // Add window to linked list
  if (_sk.window_head == nullptr) {
    _sk.window_head = window;
  }
  else {
    _sk.window_last->next = window;
  }
  _sk.window_head = window;

  return (SKWindow *)window;
}

void sk_destroy_window(SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;

  sk_make_context_current(nullptr);
  glXDestroyContext(_sk.x11.display, internal_window->x11ctx.gl_context);
  XDestroyWindow(_sk.x11.display, internal_window->x11.window);
  XFreeColormap(_sk.x11.display, internal_window->x11.colormap);

  // Remove window from list
  _SKWindow *curr = _sk.window_head;
  _SKWindow *prev = curr;
  while (curr != nullptr) {
    if (curr == internal_window) {
      prev->next = curr->next;
      break;
    }
    prev = curr;
    curr = curr->next;
  }

  delete internal_window;
}

void sk_set_window_pos(SKWindow *win, i32 x, i32 y) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  XMoveWindow(_sk.x11.display, internal_window->x11.window, x, y);
}

void sk_set_window_size(SKWindow *win, i32 width, i32 height) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  XResizeWindow(_sk.x11.display, internal_window->x11.window, (u32)width, (u32)height);
}

void sk_get_window_pos(SKWindow *win, i32 *x, i32 *y) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  XWindowAttributes attr;
  XGetWindowAttributes(_sk.x11.display, internal_window->x11.window, &attr);
  *x = attr.x;
  *y = attr.y;
}

void sk_get_window_size(SKWindow *win, i32 *width, i32 *height) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  XWindowAttributes attr;
  XGetWindowAttributes(_sk.x11.display, internal_window->x11.window, &attr);
  *width = attr.height;
  *height = attr.width;
}

bool sk_window_should_close(SKWindow *win) {
  if (!win) {
    return false;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  return internal_window->should_close;
}

_SKWindow *_sk_x11_get_window_from_id(Window id) {
  _SKWindow *win = _sk.window_head;
  while (win != nullptr) {
    if (win->x11.window == id) {
      return (_SKWindow *)win;
    }
    win = win->next;
  }
  return nullptr;
}

void sk_poll_events() {
  XEvent event;
  XNextEvent(_sk.x11.display, &event);

  switch (event.type) {
  case ClientMessage:
    if ((Atom)event.xclient.data.l[0] == _sk.x11.wm_delete_window) {
      _SKWindow *win = _sk_x11_get_window_from_id(event.xclient.window);
      win->callbacks.window_close_fun((SKWindow *)win);
    }
    break;
  default:
    break;
  }
}

void sk_set_window_close_callback(SKWindow *win, void(*func)(SKWindow *)) {
  if (!func || !win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  internal_window->callbacks.window_close_fun = func;
}

void sk_set_window_size_callback(SKWindow *win, void(*func)(SKWindow *, i32 width, i32 height)) {
  if (!func || !win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  internal_window->callbacks.window_size_fun = func;
}

void sk_set_window_pos_callback(SKWindow *win, void(*func)(SKWindow *, i32 x, i32 y)) {
  if (!func || !win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  internal_window->callbacks.window_pos_fun = func;
}

// Context
void _sk_create_context(_SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;

  internal_window->x11ctx.gl_context = glXCreateContext(_sk.x11.display, internal_window->x11.visual_info, nullptr, GL_TRUE);
  glXMakeCurrent(_sk.x11.display, internal_window->x11.window, internal_window->x11ctx.gl_context);
}

void sk_make_context_current(SKWindow *win) {
  if (!win) {
    glXMakeCurrent(_sk.x11.display, None, nullptr);
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  glXMakeCurrent(_sk.x11.display, internal_window->x11.window, internal_window->x11ctx.gl_context);
  sk_bind_renderer();
}

SKProc sk_get_proc_address(const char *proc) {
  return (SKProc)glXGetProcAddress((const unsigned char *)proc);
}

void sk_window_swap_buffers(SKWindow *win) {
  if (!win) {
    return;
  }
  _SKWindow *internal_window = (_SKWindow *)win;
  glXSwapBuffers(_sk.x11.display, internal_window->x11.window);
}

#endif