#include "GLContext.h"

#include <windows.h>

#include "Types.h"
#include "Window.h"

// TODO: Handle Window Deletion
// Maybe send app event to renderer which sends window close to rendererAPI?

namespace sk {
class Win32GLContext : public GLContext {
public:
  Win32GLContext(const Unique<Window> &window);
  ~Win32GLContext();

  void make_current() override;

private:
  HDC device_context;
  HGLRC gl_rendering_context;
  HWND window_handle;
};
} // namespace SK
