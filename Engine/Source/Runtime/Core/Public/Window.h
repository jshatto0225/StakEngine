#pragma once

#include <string>
#include <memory>

#include "Types.h"
#include "Event.h"

class Window
{
public:
    Window();
    ~Window() {}

    virtual void SetX(i32 x) = 0;
    virtual void SetY(i32 y) = 0;
    virtual void SetWidth(i32 width) = 0;
    virtual void SetHeight(i32 height) = 0;
    virtual i32 GetWidth() = 0;
    virtual i32 GetHeight() = 0;
    virtual i32 GetX() = 0;
    virtual i32 GetY() = 0;
    virtual f32 GetAspect() = 0;

    virtual bool IsOpen() = 0;
    virtual u64 GetId() final;


    virtual void SetSizeAndPos(i32 x, i32 y, i32 width, i32 height) = 0;
    virtual void Update() = 0;
    virtual void* GetHandle() = 0;
    virtual void Close() = 0;

    virtual void GenerateEvent(EventType e) final;

private:
    u64 mId;
    static u64 sCurrentId;
};
