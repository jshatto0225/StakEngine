#pragma once

#include <StakEngine.h>

#include "Editor.h"

class FEditorLayer final : public IApplicationLayer {
public:
    void OnDetach() override;
    void OnAttach() override;
    void Update() override;
    void OnImGuiRender() override;

private:
    FInputCallbackInfo EscapePressedCallbackInfo;
    FInputCallbackInfo WPressedCallbackInfo;
};
