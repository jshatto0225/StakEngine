#pragma once

#include "RHIForward.h"

#include <imgui.h>

class Rhi_Command_Context {
public:
	~Rhi_Command_Context() = default;

	virtual void begin_drawing() = 0;
	virtual void end_drawing() = 0;

	virtual void resource_transition(Rhi_Transition *transition) = 0;

	virtual void render_ingui_draw_data(ImDrawData *draw_data) = 0;

	virtual bool is_active() = 0;
};