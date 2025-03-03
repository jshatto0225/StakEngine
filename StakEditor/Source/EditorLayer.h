#pragma once

#include <StakEngine.h>

#include "Editor.h"

class FEditorLayer final : public IApplicationLayer {
public:
    FEditorLayer();

    void Update() override;
    void OnImGuiRender() override;
};
