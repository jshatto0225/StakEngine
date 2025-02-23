#pragma once

#include <StakEngine.h>

#include "Editor.h"

class Editor_Layer final : public App_Layer {
public:
    Editor_Layer();

    void update() override;
    void on_imgui_render() override;
};
