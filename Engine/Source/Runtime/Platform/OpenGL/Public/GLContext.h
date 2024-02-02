#pragma once

class GLContext
{
public:
    GLContext() = default;
    virtual ~GLContext() {}

    virtual void MakeCurrent() = 0;
};
