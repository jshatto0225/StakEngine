#pragma once

#include "RHIForward.h"

#include <imgui.h>

class IRHICommandContext {
public:
	~IRHICommandContext() = default;

	virtual void BeginDrawing() = 0;
	virtual void EndDrawing() = 0;

	virtual void ResourceTransition(IRHITransition *Transition) = 0;

	virtual void RenderImGuiDrawData(ImDrawData *DrawData) = 0;

	virtual FBool IsActive() = 0;
};