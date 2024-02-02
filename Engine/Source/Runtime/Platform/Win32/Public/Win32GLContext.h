#include "GLContext.h"

#include <windows.h>
#include <unordered_set>

#include "Window.h"
#include "Types.h"

// TODO: Handle Window Deletion
// Maby send app event to renderer which sends window close to rendererAPI?

namespace SK
{
    class Win32GLContext : public GLContext
    {
    public:
        Win32GLContext(const Unique<Window>& window);
        ~Win32GLContext();

        void MakeCurrent() override;

    private:
        HDC mDeviceContext;
        HGLRC mGLRenderingContext;
        HWND mWindowHandle;
    };
}

