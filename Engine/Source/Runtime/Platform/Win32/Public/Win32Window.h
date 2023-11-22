#include "Window.h"

#include <windows.h>

class Win32Window final : public Window
{
public:
    Win32Window(const std::string& name, i32 x, i32 y, i32 width, i32 height);
    ~Win32Window();

    void SetSizeAndPos(i32 x, i32 y, i32 width, i32 height) override;

    void Update() override;
    inline void* GetHandle() override
    {
        return (void*)mWindowHandle;
    }

    void Close() override;

private:
    HWND mWindowHandle;
    HINSTANCE mInstanceHandle;
};