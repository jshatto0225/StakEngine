#pragma once

class ApplicationLayer
{
public:
    virtual void Start() {}
    virtual void End() {}
    virtual void Update() {}
    virtual void OnEvent() {}
};
