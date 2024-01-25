#include "GLContext.h"

#include <windows.h>
#include <unordered_set>

#include "Window.h"
#include "Types.h"

// TODO: Handle Window Deletion
// Maby send app event to renderer which sends window close to rendererAPI?

class Win32GLContext : public GLContext
{
public:
    Win32GLContext(u64 window);

    void MakeCurrent() override;
    
private:
    HDC mDeviceContext;
    HGLRC mGLRenderingContext;
    u64 mWindow;
};

