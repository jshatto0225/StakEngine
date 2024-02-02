#pragma once

#include "Event.h"

namespace SK
{
    class ApplicationLayer
    {
    public:
        virtual void Start() {}
        virtual void End() {}
        virtual void Update() {}
        virtual void OnEvent(Event& e) {}
    };
}

