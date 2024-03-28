#include "Renderer.h"

#include "Platform.h"
#include "Log.h"
#include "StakMath.h"
#include "Renderer2D.h"

u32
ShaderDataTypeSize(shader_data_type Type)
{
  switch (Type)
  {
  case SDT_FLOAT:
    return 4;
  case SDT_FLOAT2:
    return 4 * 2;
  case SDT_FLOAT3:
    return 4 * 3;
  case SDT_FLOAT4:
    return 4 * 4;
  case SDT_MAT3:
    return 4 * 3 * 3;
  case SDT_MAT4:
    return 4 * 4 * 4;
  case SDT_INT:
    return 4;
  case SDT_INT2:
    return 4 * 2;
  case SDT_INT3:
    return 4 * 3;
  case SDT_INT4:
    return 4 * 4;
  case SDT_BOOL:
    return 1;
  }
  return 0;
}

u32
GetBufferElementComponentCount(const buffer_element *BufferElement)
{
  switch (BufferElement->Type)
  {
    case SDT_FLOAT:
      return 1;
    case SDT_FLOAT2:
      return 2;
    case SDT_FLOAT3:
      return 3;
    case SDT_FLOAT4:
      return 4;
    case SDT_MAT3:
      return 3;
    case SDT_MAT4:
      return 4;
    case SDT_INT:
      return 1;
    case SDT_INT2:
      return 2;
    case SDT_INT3:
      return 3;
    case SDT_INT4:
      return 4;
    case SDT_BOOL:
      return 1;
    default:
      return 0;
  }
}

buffer_layout *
CreateBufferLayout(u64 Size)
{
  buffer_layout *BufferLayout = (buffer_layout *)malloc(sizeof(buffer_layout) +
                                                        Size *
                                                        sizeof(buffer_element));

  if (!BufferLayout)
  {
    LogCoreError("Failed to allocate memory for buffer layout");
    return NULL;
  }

  BufferLayout->Size = Size;
  BufferLayout->Stride = 0;
  BufferLayout->Elements = (buffer_element *)(BufferLayout + 1);

  return BufferLayout;
}

void
AddElementToLayout(buffer_layout *Layout, u64 Position, shader_data_type Type, const char *Name, bool Normalized)
{
  Layout->Elements[Position].Type = Type;
  Layout->Elements[Position].Name = Name;
  Layout->Elements[Position].Normalized = Normalized;
  Layout->Elements[Position].Offset = 0;
  Layout->Elements[Position].Size = ShaderDataTypeSize(Type);
}

void
CalculateOffsetsAndStride(buffer_layout *Layout)
{
  u64 Offset = 0;
  Layout->Stride = 0;
  for (u64 i = 0; i < Layout->Size; i++) {
    Layout->Elements[i].Offset = Offset;
    Offset += Layout->Elements[i].Size;
    Layout->Stride += Layout->Elements[i].Size;
  }
}

void
DestroyBufferLayout(buffer_layout **BufferLayout)
{
  if (*BufferLayout != NULL)
  {
    free(*BufferLayout);
    *BufferLayout = NULL;
  }
}

void
RenderCommandInit(window *Window)
{
  RenderApiInit(Window);
}

void
RenderCommandShutdown()
{
  RenderApiShutdown();
}

void
RenderCommandSwapBuffers()
{
  RenderApiSwapBuffers();
}

void
RenderCommandSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha)
{
  RenderApiSetClearColor(Red, Green, Blue, Alpha);
}

void
RenderCommandClear() {
  RenderApiClear();
}

void
RenderCommandSetViewport(i32 X, i32 Y, i32 Width, i32 Height)
{
  RenderApiSetViewport(X, Y, Width, Height);
}

void
RenderCommandSetLineWidth(f32 Width)
{
  RenderApiSetLineWidth(Width);
}

void
RenderCommandBind() {
  RenderApiBind();
}

void
RenderCommandDrawIndexed(vertex_array *VertexArray, u32 Count)
{
  RenderApiDrawIndexed(VertexArray, Count);
}

void
RenderCommandDrawLines(vertex_array *VertexArray, u32 Count)
{
  RenderApiDrawLines(VertexArray, Count);
}

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
  RenderApiShutdown();
}

void
RendererOnWindowResize(i32 Width, i32 Height)
{
  RenderApiSetViewport(0, 0, Width, Height);
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
