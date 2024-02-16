#include "LinuxWindow.h"

#include "Log.h"

namespace sk {
LinuxWindow::LinuxWindow(const std::string &name, i32 x, i32 y, i32 width, i32 height) {
  this->name = name;
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;

  display = XOpenDisplay(nullptr);
  if (!display) {
    SK_CORE_ERROR("Display not created");
    return;
  }
  screen = DefaultScreen(display);
  root = RootWindow(display, screen);
  window = XCreateSimpleWindow(
    display,
    root,
    x,
    y,
    width,
    height,
    10 /* BORDER */,
    BlackPixel(display, screen),
    WhitePixel(display, screen));
  XMapWindow(display, window);
}

LinuxWindow::~LinuxWindow() {
  XUnmapWindow(display, window);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
}

void LinuxWindow::set_x(i32 x) {}

void LinuxWindow::set_y(i32 y) {}

void LinuxWindow::set_width(i32 width) {}

void LinuxWindow::set_height(i32 height) {}

i32 LinuxWindow::get_width() {
  return width;
}

i32 LinuxWindow::get_height() {
  return height;
}

i32 LinuxWindow::get_x() {
  return x;
}

i32 LinuxWindow::get_y() {
  return y;
}

f32 LinuxWindow::get_aspect() {
  return (f32)width / (f32)height;
}

bool LinuxWindow::is_open() {
  return open;
}

void LinuxWindow::set_size_and_pos(i32 x, i32 y, i32 width, i32 height) {}

void LinuxWindow::update() {
  XEvent event;
  XNextEvent(display, &event);
}

void LinuxWindow::close() {}
} // namespace SK
