#include "Window.h"

#include <windows.h>

class Win32Window final : public Window
{
public:
    Win32Window(const std::string& name, i32 x, i32 y, i32 width, i32 height);
    ~Win32Window();

    void SetSizeAndPos(i32 x, i32 y, i32 width, i32 height) override;
    void SetX(i32 x) override;
    void SetY(i32 y) override;
    void SetWidth(i32 width) override;
    void SetHeight(i32 height) override;
    i32 GetWidth() override;
    i32 GetHeight() override;
    i32 GetX() override;
    i32 GetY() override;
    f32 GetAspect() override;

    void Update() override;
    void* GetHandle() override;
    bool IsOpen() override;
    void Close() override;

private:
    HWND mWindowHandle;
    HINSTANCE mInstanceHandle;

private:
    bool mOpen;
    std::string mName;
    i32 mX;
    i32 mY;
    i32 mWidth;
    i32 mHeight;
};