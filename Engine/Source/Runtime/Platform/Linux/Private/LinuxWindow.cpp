#include "LinuxWindow.h"

namespace sk {
LinuxWindow::LinuxWindow(const std::string &name, i32 x, i32 y, i32 width, i32 height) {}

LinuxWindow::~LinuxWindow() {}

void LinuxWindow::set_x(i32 x) {}

void LinuxWindow::set_y(i32 y) {}
void LinuxWindow::set_width(i32 width) {}
void LinuxWindow::set_height(i32 height) {}
i32 LinuxWindow::get_width() { return 0; }
i32 LinuxWindow::get_height() { return 0; }
i32 LinuxWindow::get_x() { return 0; }
i32 LinuxWindow::get_y() { return 0; }
f32 LinuxWindow::get_aspect() { return 0; }

bool LinuxWindow::is_open() { return false; }

void LinuxWindow::set_size_and_pos(i32 x, i32 y, i32 width, i32 height) {}
void LinuxWindow::update() {}
void *LinuxWindow::get_handle() { return 0; }
void LinuxWindow::close() {}
} // namespace SK
