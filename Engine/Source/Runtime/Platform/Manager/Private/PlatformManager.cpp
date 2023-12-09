#include "PlatformManager.h"

#ifdef WIN32
#include "Win32Window.h"
#define PLATFORM_WINDOW Win32Window
#include "Win32InputManager.h"
#define PLATFORM_INPUT_MANAGER Win32InputManager
#include "VulkanRenderer.h"
#define PLATFORM_RENDERER VulkanRenderer
#endif

Unique<Window> PlatformManager::NewWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
    return MakeUnique<PLATFORM_WINDOW>(name, x, y, width, height);
}

Unique<Renderer> PlatformManager::NewRenderer()
{
    return MakeUnique<PLATFORM_RENDERER>();
}

Unique<InputManager> PlatformManager::NewInputManager()
{
    return MakeUnique<PLATFORM_INPUT_MANAGER>();
}
