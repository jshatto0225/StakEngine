#pragma once

#include "../StakEngine/StakEngine.h"

bool UIRendererCheckBox(const char *label);
const char *UIRendererTextBox();
bool UIRendererTextButton(const char *text, const font *Font);
bool UIRendererImageButton(const texture2d *image);
void UIRendererText(const char *text, const font *Font);