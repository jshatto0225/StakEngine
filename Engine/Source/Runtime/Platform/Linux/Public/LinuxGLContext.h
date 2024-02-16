#pragma once

#include <GL/glx.h>
using X11Window = Window;

#include "GLContext.h"
#include "Window.h"

namespace sk {
class LinuxGLContext : public GLContext {
public:
  LinuxGLContext(const Unique<Window> &window);
  void make_current() override;

private:
  GLXContext context;
  Display *display;
  X11Window window;
};
}
