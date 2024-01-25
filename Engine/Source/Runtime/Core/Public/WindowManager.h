#pragma once

#include <unordered_set>
#include <unordered_map>

#include "Window.h"

class WindowManager final
{
public:
    static void Init();
    static void Shutdown();
    static void Update();

    static u64 AddWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height);
    static bool HasWindow(u64 id);
    
    static void SetWindowX(u64 id, i32 x);
    static void SetWindowY(u64 id, i32 y);
    static void SetWindowWidth(u64 id, i32 width);
    static void SetWindowHeight(u64 id, i32 height);
    static i32 GetWindowWidth(u64 id);
    static i32 GetWindowHeight(u64 id);
    static i32 GetWindowX(u64 id);
    static i32 GetWindowY(u64 id);
    static f32 GetWindowAspect(u64 id);

    static bool IsWindowOpen(u64 id);

    static void SetWindowSizeAndPos(u64 id, i32 x, i32 y, i32 width, i32 height);
    static void* GetWindowHandle(u64 id);
    static void CloseWindow(u64 id);

private:
    static bool sRunning;
    static std::unordered_map<u64, Unique<Window>> sIdWindowMap;
};
