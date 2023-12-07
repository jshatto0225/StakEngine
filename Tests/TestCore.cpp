//#define CATCH_CONFIG_RUNNER

#include "catch.hpp"

#include "Runtime/Runtime.h"

//TODO: Create Test layer
//TODO: Cross Platform tests

class TestLayer : public ApplicationLayer
{
public:
    void Start() override {}
    void End() override {}
    void Update() override {}
    void OnEvent(Event& e) override {}
};
