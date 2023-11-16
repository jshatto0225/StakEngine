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

    virtual void SetSizeAndPos(i32 x, i32 y, i32 width, i32 height) = 0;

    inline void SetX(i32 x)
    {
        this->x = x;
        SetSizeAndPos(x, y, width, height);
    }

    inline void SetY(i32 y)
    {
        this->y = y;
        SetSizeAndPos(x, y, width, height);
    }

    inline void SetWidth(i32 width) 
    { 
        this->width = width; 
        SetSizeAndPos(x, y, width, height); 
    }

    inline void SetHeight(i32 height)
    {
        this->height = height;
        SetSizeAndPos(x, y, width, height);
    }

    inline i32 GetWidth() 
    {
        return width;
    }

    inline i32 GetHeight() 
    {
        return height;
    }

    inline f32 GetAspect() 
    {
        return width / height;
    }

    inline bool IsOpen() 
    { 
        return open;
    }

    virtual void Update() = 0;
    virtual void* GetHandle() = 0;
    inline u64 GetId() 
    { 
        return id; 
    }

    virtual void Close() = 0;

    void GenerateEvent(EventType e);

protected:
    bool open;
    std::string name;
    i32 x;
    i32 y;
    i32 width;
    i32 height;

private:
    u64 id;
    static u64 CurrentId;
};

std::unique_ptr<Window> MakeWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height);
void KillWindowManager();
