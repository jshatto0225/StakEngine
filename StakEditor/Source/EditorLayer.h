#pragma once

#include <StakEngine.h>

#include "Editor.h"

class FEditorLayer final : public IApplicationLayer {
public:
  FEditorLayer(TRef<const IInput> Input);

  void Update() override;
  void OnImGuiRender() override;

private:
  TRef<const IInput> mInput;
};
