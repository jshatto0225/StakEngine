#pragma once

#include "Types.h"
#include "StakMath.h"
#include "Font.h"
#include "Camera.h"
#include "Window.h"
#include "Shader.h"
#include "VertexArray.h"

void RendererInit(window *Window);
void RendererShutdown();
void RendererOnWindowResize(i32 Width, i32 Height);
void RendererBeginScene();
void RendererEndScene();
void RendererSubmit(shader *Shader, vertex_array *VertexArray);
