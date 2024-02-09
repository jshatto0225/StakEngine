#pragma once

#include "GLContext.h"
#include "Window.h"

namespace sk {
class LinuxGLContext : public GLContext {
public:
  LinuxGLContext(const Unique<Window> &window);
  void make_current() override;
};
}
