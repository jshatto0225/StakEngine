#if defined(SK_X11)

#include "Platform.h"
#include "Window.h"
#include "Renderer.h"
#include "Event.h"

namespace sk {
bool Platform::initialized = false;
X11Platform Platform::x11;

void Platform::init() {
  if (Platform::initialized) {
    return;
  }
  Platform::x11.display = XOpenDisplay(nullptr);
  Platform::x11.wm_delete_window = XInternAtom(Platform::x11.display, "WM_DELETE_WINDOW", false);
}

bool Platform::is_initialized() {
  return Platform::initialized;
}

Proc Platform::get_proc_address(const char *name) {
  return (Proc)glXGetProcAddress((const unsigned char *)name);
}

Context::Context(Window *win) {
  this->x11.context = glXCreateContext(Platform::x11.display, win->data.x11.visual_info, nullptr, GL_TRUE);
  glXMakeCurrent(Platform::x11.display, win->data.x11.window, this->x11.context);
}

Context::~Context() {
  glXMakeCurrent(Platform::x11.display, None, nullptr);
  glXDestroyContext(Platform::x11.display, this->x11.context);
}

void Context::make_current() {
  glXMakeCurrent(Platform::x11.display, this->window->data.x11.window, this->x11.context);
  RenderApi::bind();
}

Window::Window(const WindowConfig &config) {
  if (!Platform::is_initialized()) {
    Platform::init();
  }
  this->data.x = config.x;
  this->data.y = config.y;
  this->data.width = config.width;
  this->data.height = config.height;
  this->data.title = config.title;
  this->data.should_close = false;
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
  this->context = new Context(this);
}

Window::~Window() {
  delete this->context;
  XDestroyWindow(Platform::x11.display, this->data.x11.window);
  XFreeColormap(Platform::x11.display, this->data.x11.colormap);
}

void Window::set_event_callback(const std::function<void(Event &)> &func) {
  this->data.event_function = func;
}

void Window::set_pos(i32 x, i32 y) {
  XMoveWindow(Platform::x11.display, this->data.x11.window, x, y);
}

void Window::set_size(i32 width, i32 height) {
  XResizeWindow(Platform::x11.display, this->data.x11.window, (u32)width, (u32)height);
}

void Window::get_pos(i32 *x, i32 *y) {
  XWindowAttributes attr;
  XGetWindowAttributes(Platform::x11.display, this->data.x11.window, &attr);
  *x = attr.x;
  *y = attr.y;
}

void Window::get_size(i32 *width, i32 *height) {
  XWindowAttributes attr;
  XGetWindowAttributes(Platform::x11.display, this->data.x11.window, &attr);
  *width = attr.height;
  *height = attr.width;
}

void Window::update() {
  XEvent event;
  XNextEvent(Platform::x11.display, &event);
  switch (event.type) {
  case ClientMessage:
    if ((Atom)event.xclient.data.l[0] == Platform::x11.wm_delete_window) {
      Event e = {};
      e.type = WINDOW_CLOSE;
      e.win_close_event.window = this;
      this->data.event_function(e);
    }
    break;
  default:
    break;
  }

  glXSwapBuffers(Platform::x11.display, this->data.x11.window);
}
}

#endif