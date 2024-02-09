#pragma once

#include "Window.h"

namespace sk {
class LinuxWindow : public Window {
public:
  LinuxWindow(const std::string &name, i32 x, i32 y, i32 width, i32 height);
  ~LinuxWindow();

  void set_x(i32 x) override;
  void set_y(i32 y) override;
  void set_width(i32 width) override;
  void set_height(i32 height) override;
  i32 get_width() override;
  i32 get_height() override;
  i32 get_x() override;
  i32 get_y() override;
  f32 get_aspect() override;

  bool is_open() override;

  void set_size_and_pos(i32 x, i32 y, i32 width, i32 height) override;
  void update() override;
  void *get_handle() override;
  void close() override;

private:
  bool open;
};
}
