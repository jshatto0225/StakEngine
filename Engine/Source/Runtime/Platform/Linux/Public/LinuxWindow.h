#include "Window.h"

#include <X11/Xlib.h>

class LinuxWindow : public Window
{
public:
    LinuxWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height);
    ~LinuxWindow();

    void SetX(i32 x) override;
    void SetY(i32 y) override;
    void SetWidth(i32 width) override;
    void SetHeight(i32 height) override;
    i32 GetWidth() override;
    i32 GetHeight() override;
    i32 GetX() override;
    i32 GetY() override;
    f32 GetAspect() override;

    bool IsOpen() override;

    void SetSizeAndPos(i32 x, i32 y, i32 width, i32 height) override;
    void Update() override;
    void* GetHandle() override;
    void Close() override;

private:
    bool mOpen;
};

