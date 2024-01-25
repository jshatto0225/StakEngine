#pragma once

#include "Window.h"

class GLContext
{
public:
    GLContext() = default;
    virtual ~GLContext() {}

    virtual void MakeCurrent() = 0;
};
