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

    inline virtual void SetX(i32 x) final
    {
        mX = x;
        SetSizeAndPos(x, mY, mWidth, mHeight);
    }

    inline virtual void SetY(i32 y) final
    {
        mY = y;
        SetSizeAndPos(mX, y, mWidth, mHeight);
    }

    inline virtual void SetWidth(i32 width) final 
    { 
        mWidth = width; 
        SetSizeAndPos(mX, mY, width, mHeight); 
    }

    inline virtual void SetHeight(i32 height) final
    {
        mHeight = height;
        SetSizeAndPos(mX, mY, mWidth, height);
    }

    inline virtual i32 GetWidth() final 
    {
        return mWidth;
    }

    inline virtual i32 GetHeight() final 
    {
        return mHeight;
    }

    inline virtual f32 GetAspect() final 
    {
        return mWidth / mHeight;
    }

    inline virtual bool IsOpen() final
    { 
        return mOpen;
    }
    inline virtual u64 GetId() final 
    { 
        return mId;
    }

    virtual void GenerateEvent(EventType e) final;

    virtual void SetSizeAndPos(i32 x, i32 y, i32 width, i32 height) = 0;
    virtual void Update() = 0;
    virtual void* GetHandle() = 0;
    virtual void Close() = 0;

protected:
    bool mOpen;
    std::string mName;
    i32 mX;
    i32 mY;
    i32 mWidth;
    i32 mHeight;

private:
    u64 mId;
    static u64 sCurrentId;
};

void KillWindowManager();
