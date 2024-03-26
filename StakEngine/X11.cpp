#if defined(SK_X11)

#include "Platform.h"
#include "Window.h"
#include "Renderer.h"

namespace sk {
bool Platform::initialized = false;
X11Platform Platform::x11;

void Platform::Init() {
  if (Platform::initialized) {
    return;
  }
  Platform::x11.display = XOpenDisplay(nullptr);
  Platform::x11.wm_delete_window = XInternAtom(Platform::x11.display, "WM_DELETE_WINDOW", False);
}

bool Platform::IsInitialized() {
  return Platform::initialized;
}

Proc Platform::GetProcAddress(const char *name) {
  return (Proc)glXGetProcAddress((const unsigned char *)name);
}

Context::Context(const Window *win) {
  this->x11.context = glXCreateContext(Platform::x11.display, win->data.x11.visual_info, nullptr, GL_TRUE);
  glXMakeCurrent(Platform::x11.display, win->data.x11.window, this->x11.context);
  RenderApi::Bind();
}

Context::~Context() {
  glXMakeCurrent(Platform::x11.display, None, nullptr);
  glXDestroyContext(Platform::x11.display, this->x11.context);
}

void Context::makeCurrent() {
  glXMakeCurrent(Platform::x11.display, this->window->data.x11.window, this->x11.context);
  RenderApi::Bind();
}

Window::Window(const WindowConfig &config) {
  if (!Platform::IsInitialized()) {
    Platform::Init();
  }
  this->data.x = config.x;
  this->data.y = config.y;
  this->data.width = config.width;
  this->data.height = config.height;
  this->data.title = config.title;
  this->data.x11.root = DefaultRootWindow(Platform::x11.display);

  int attribs[] = {
    GLX_RGBA,
    GLX_DEPTH_SIZE,
    GLX_DOUBLEBUFFER,
    None
  };
  this->data.x11.visual_info = glXChooseVisual(Platform::x11.display, DefaultScreen(Platform::x11.display), attribs);
  this->data.x11.colormap = XCreateColormap(Platform::x11.display, this->data.x11.root, this->data.x11.visual_info->visual, AllocNone);
  this->data.x11.attribs.colormap = this->data.x11.colormap;
  this->data.x11.attribs.event_mask = ExposureMask | KeyPressMask;

  this->data.x11.window = XCreateSimpleWindow(Platform::x11.display,
                                              this->data.x11.root,
                                              this->data.x,
                                              this->data.y,
                                              (u32)this->data.width,
                                              (u32)this->data.height,
                                              0,
                                              0,
                                              0xffffffff);
  XMapWindow(Platform::x11.display, this->data.x11.window);
  XSetWMProtocols(Platform::x11.display, this->data.x11.window, &Platform::x11.wm_delete_window, 1);
  XSelectInput(Platform::x11.display, this->data.x11.window, StructureNotifyMask);
  this->context = std::make_unique<Context>(this);
}

Window::~Window() {
  XDestroyWindow(Platform::x11.display, this->data.x11.window);
  XFreeColormap(Platform::x11.display, this->data.x11.colormap);
}

void Window::setEventCallback(const std::function<void(Event &)> &func) {
  this->data.event_function = func;
}

void Window::setPos(i32 x, i32 y) {
  XMoveWindow(Platform::x11.display, this->data.x11.window, x, y);
}

void Window::setSize(i32 width, i32 height) {
  XResizeWindow(Platform::x11.display, this->data.x11.window, (u32)width, (u32)height);
}

Vec2 Window::getPos() {
  XWindowAttributes attr;
  XGetWindowAttributes(Platform::x11.display, this->data.x11.window, &attr);
  return Vec2(attr.x, attr.y);
}

Vec2 Window::getSize() {
  XWindowAttributes attr;
  XGetWindowAttributes(Platform::x11.display, this->data.x11.window, &attr);
  return Vec2(attr.height, attr.width);
}

void Window::update() {
  XEvent event;
  XNextEvent(Platform::x11.display, &event);
  switch (event.type) {
  case ClientMessage:
    if (event.xclient.data.l[0] == Platform::x11.wm_delete_window) {
      Event e = {};
      e.type = WINDOW_CLOSE;
      e.win_close_event.window = this;
      this->data.event_function(e);
    }
    break;
  case ConfigureNotify: {
    if (event.xconfigure.width != this->data.width || event.xconfigure.height != this->data.height) {
      Event e = {};
      e.type = WINDOW_RESIZED;
      e.win_resize_event.window = this;
      e.win_resize_event.width = event.xconfigure.width;
      e.win_resize_event.height = event.xconfigure.height;
      this->data.event_function(e);
    }
    if (event.xconfigure.x != this->data.x || event.xconfigure.y != this->data.y) {
      Event e = {};
      e.type = WINDOW_MOVED;
      e.win_move_event.window = this;
      e.win_move_event.x = event.xconfigure.x;
      e.win_move_event.y = event.xconfigure.y;
      this->data.event_function(e);
    }
    break;
  }
  default:
    break;
  }

  glXSwapBuffers(Platform::x11.display, this->data.x11.window);
}

} // namespace sk

#endif
