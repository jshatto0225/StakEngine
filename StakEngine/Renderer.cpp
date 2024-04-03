#include "Renderer.h"

#include "Platform.h"
#include "Log.h"
#include "StakMath.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

/********************
 * Public Interface *
 ********************/

void
RendererInit(window *Window)
{
  RenderCommandInit(Window);
  Renderer2DInit();
}

void
RendererShutdown()
{
  Renderer2DShutdown();
  RenderCommandShutdown();
}

void
RendererOnWindowResize(i32 Width, i32 Height)
{
  RenderCommandSetViewport(0, 0, Width, Height);
}

void
RendererBeginScene()
{

}

void
RendererEndScene()
{

}

void
RendererSubmit(shader *Shader, vertex_array *VertexArray)
{
  BindShader(Shader);
  BindVertexArray(VertexArray);
  const index_buffer *IndexBuffer = GetVertexArrayIndexBuffer(VertexArray);
  RenderCommandDrawIndexed(VertexArray, GetIndexBufferIndexCount(IndexBuffer));
}
