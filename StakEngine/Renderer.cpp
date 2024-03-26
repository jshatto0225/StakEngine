#include "Renderer.h"

#include "Platform.h"
#include "Log.h"
#include "StakMath.h"

u32 ShaderDataTypeSize(shader_data_type Type) {
  switch (Type) {
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

u32 GetBufferElementComponentCount(const buffer_element *BufferElement) {
  switch (BufferElement->Type) {
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

buffer_layout *CreateBufferLayout(u64 Size) {
  buffer_layout *BufferLayout = (buffer_layout *)malloc(sizeof(buffer_layout) +
                                                        Size *
                                                        sizeof(buffer_element));

  if (!BufferLayout) {
    LogCoreError("Failed to allocate memory for buffer layout");
    return NULL;
  }

  BufferLayout->Size = Size;
  BufferLayout->Stride = 0;
  BufferLayout->Elements = (buffer_element *)(BufferLayout + 1);

  return BufferLayout;
}

void AddElementToLayout(buffer_layout *Layout,
                        u64 Position,
                        shader_data_type Type,
                        const char *Name,
                        bool Normalized) {
  Layout->Elements[Position].Type = Type;
  Layout->Elements[Position].Name = Name;
  Layout->Elements[Position].Normalized = Normalized;
  Layout->Elements[Position].Offset = 0;
  Layout->Elements[Position].Size = ShaderDataTypeSize(Type);
}

void CalculateOffsetsAndStride(buffer_layout *Layout) {
  u64 Offset = 0;
  Layout->Stride = 0;
  for (u64 i = 0; i < Layout->Size; i++) {
    Layout->Elements[i].Offset = Offset;
    Offset += Layout->Elements[i].Size;
    Layout->Stride += Layout->Elements[i].Size;
  }
}

void DestroyBufferLayout(buffer_layout **BufferLayout) {
  if (*BufferLayout) {
    free(*BufferLayout);
    *BufferLayout = NULL;
  }
}

void RenderCommandInit(window *Window) {
  RenderApiInit(Window);
}

void RenderCommandShutdown() {
  RenderApiShutdown();
}

void RenderCommandSwapBuffers() {
  RenderApiSwapBuffers();
}

void RenderCommandSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha) {
  RenderApiSetClearColor(Red, Green, Blue, Alpha);
}

void RenderCommandClear() {
  RenderApiClear();
}

void RenderCommandSetViewport(i32 X, i32 Y, i32 Width, i32 Height) {
  RenderApiSetViewport(X, Y, Width, Height);
}

void RenderCommandSetLineWidth(f32 Width) {
  RenderApiSetLineWidth(Width);
}

void RenderCommandBind() {
  RenderApiBind();
}

void RenderCommandDrawIndexed(vertex_array *VertexArray, u32 Count) {
  RenderApiDrawIndexed(VertexArray, Count);
}

void RenderCommandDrawLines(vertex_array *VertexArray, u32 Count) {
  RenderApiDrawLines(VertexArray, Count);
}

void RendererInit(window *Window) {
  RenderCommandInit(Window);
  Renderer2DInit();
}

void RendererShutdown() {
  Renderer2DShutdown();
  RenderApiShutdown();
}

void RendererOnWindowResize(i32 Width, i32 Height) {
  RenderApiSetViewport(0, 0, Width, Height);
}

void RendererBeginScene() {

}

void RendererEndScene() {

}

void RendererSubmit(shader *Shader, vertex_array *VertexArray) {
  BindShader(Shader);
  BindVertexArray(VertexArray);
  const index_buffer *IndexBuffer = GetVertexArrayIndexBuffer(VertexArray);
  RenderCommandDrawIndexed(VertexArray, GetIndexBufferIndexCount(IndexBuffer));
}

struct quad_vertex {
  vec3 Pos;
  vec2 TexCoord;
  f32 TexIndex;
};

struct circle_vertex {
  // TODO:
};

struct line_vertex {
  // TODO:
};

struct text_vertex {
  // TODO:
};

struct camera_data {
  mat4 ViewProj;
};

struct renderer2d_data {
  static constexpr u32 MaxQuads = 1000;
  static constexpr u32 VerticesPerQuad = 4;
  static constexpr u32 IndicesPerQuad = 6;
  static constexpr u32 MaxQuadVertices = MaxQuads * VerticesPerQuad;
  static constexpr u32 MaxQuadIndices = MaxQuads * IndicesPerQuad;
  static constexpr u32 MaxTextureSlots = 32;

  quad_vertex *QuadVerticesBase;
  quad_vertex *QuadVertexCurrent;
  u32 QuadIndexCount;

  vertex_buffer *QuadVertexBuffer;
  vertex_array *QuadVertexArray;
  shader *QuadShader;

  texture2d *WhiteTexture;
  const texture2d *TextureSlots[MaxTextureSlots];
  texture2d *RendererTextures[MaxTextureSlots];
  u32 RendererTextureCount;

  u32 TextureSlotIndex;

  camera_data CameraData;
  uniform_buffer *CameraUniformBuffer;

  vec3 QuadVertexPositions[4];
  vec2 QuadTexCoords[4];
};

renderer2d_data Data;

void Renderer2DFlush() {
  if (Data.QuadIndexCount) {
    u32 DataSize = (u32)((u8 *)Data.QuadVertexCurrent - (u8 *)Data.QuadVerticesBase);
    SetVertexBufferData(Data.QuadVertexBuffer, Data.QuadVerticesBase, DataSize);

    for (u32 i = 0; i < Data.TextureSlotIndex; i++) {
      BindTexture2D(Data.TextureSlots[i], i);
    }

    BindShader(Data.QuadShader);

    RenderCommandDrawIndexed(Data.QuadVertexArray, Data.QuadIndexCount);
  }
}

void Renderer2DStartBatch() {
  Data.QuadIndexCount = 0;
  Data.QuadVertexCurrent = Data.QuadVerticesBase;

  for (i32 i = 0; i < Data.RendererTextureCount; i++) {
    DestroyTexture2D(&Data.RendererTextures[i]);
  }

  Data.TextureSlotIndex = 1;
}

void Renderer2DNextBatch() {
  Renderer2DFlush();
  Renderer2DStartBatch();
}

void Renderer2DInit() {
  u32 *QuadIndices = (u32 *)malloc(Data.MaxQuadIndices * sizeof(u32));

  if (!QuadIndices) {
    LogCoreError("Failed to allocate memory for quad incices");
    return;
  }
  
  u32 Offset = 0;
  for (u32 i = 0; i < Data.MaxQuadIndices; i += Data.IndicesPerQuad) {
    QuadIndices[i + 0] = Offset;
    QuadIndices[i + 1] = Offset + 1;
    QuadIndices[i + 2] = Offset + 3;
    QuadIndices[i + 3] = Offset + 1;
    QuadIndices[i + 4] = Offset + 2;
    QuadIndices[i + 5] = Offset + 3;
    Offset += Data.VerticesPerQuad;
  }

  Data.QuadIndexCount = 0;
  Data.QuadVerticesBase = (quad_vertex *)malloc(Data.MaxQuadVertices * sizeof(quad_vertex));
  Data.QuadVertexCurrent = Data.QuadVerticesBase;

  Data.QuadShader = CreateShader(SHADER_DIR "QuadShader.glsl");

  Data.QuadVertexBuffer = CreateVertexBuffer((u32)sizeof(quad_vertex) * Data.MaxQuadVertices);
  buffer_layout *BufferLayout = CreateBufferLayout(3);
  AddElementToLayout(BufferLayout, 0, SDT_FLOAT3, "aPosition", false);
  AddElementToLayout(BufferLayout, 1, SDT_FLOAT2, "aTexCoord", false);
  AddElementToLayout(BufferLayout, 2, SDT_FLOAT,  "aTexIndex", false);
  CalculateOffsetsAndStride(BufferLayout);
  SetVertexBufferLayout(Data.QuadVertexBuffer, &BufferLayout);


  index_buffer *QuadIndexBuffer = CreateIndexBuffer(QuadIndices, Data.MaxQuadIndices);

  free(QuadIndices);

  Data.QuadVertexArray = CreateVertexArray();
  AddVertexBufferToVertexArray(Data.QuadVertexArray, Data.QuadVertexBuffer);
  SetVertexArrayIndexBuffer(Data.QuadVertexArray, &QuadIndexBuffer);

  Data.TextureSlotIndex = 1;
  texture_specification Spec = { 1, 1, IMAGE_FORMAT_RGBA8, false };
  Data.WhiteTexture = CreateTexture2D(&Spec);
  u32 WhiteColor = 0xffffffff;
  SetTexture2DData(Data.WhiteTexture, &WhiteColor, sizeof(WhiteColor));
  Data.TextureSlots[0] = Data.WhiteTexture;
  Data.RendererTextureCount = 0;
  
  Data.CameraUniformBuffer = CreateUniformBuffer((u32)sizeof(camera_data), 0);
  
  Data.QuadVertexPositions[0] = {  0.5f,  0.5f, 0.0f };
  Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f };
  Data.QuadVertexPositions[2] = { -0.5f, -0.5f, 0.0f };
  Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f };

  Data.QuadTexCoords[0] = { 1.0f, 1.0f };
  Data.QuadTexCoords[1] = { 1.0f, 0.0f };
  Data.QuadTexCoords[2] = { 0.0f, 0.0f };
  Data.QuadTexCoords[3] = { 0.0f, 1.0f };
}

void Renderer2DShutdown() {
  free(Data.QuadVerticesBase);
  DestroyShader(&Data.QuadShader);
  DestroyTexture2D(&Data.WhiteTexture);
  DestroyVertexBuffer(&Data.QuadVertexBuffer);
  DestroyVertexArray(&Data.QuadVertexArray);
  DestroyUniformBuffer(&Data.CameraUniformBuffer);

  for (i32 i = 0; i < Data.RendererTextureCount; i++) {
    DestroyTexture2D(&Data.RendererTextures[i]);
  }
}

void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, const vec4 *Color) {
  Renderer2DDrawQuad(Pos, Size, 0, Color);
}

void Renderer2DDrawQuad(const vec2 *Pos,
                        const vec2 *Size,
                        texture2d *Tex,
                        bool TransferTextureOwnership) {
  Renderer2DDrawQuad(Pos, Size, 0, Tex, TransferTextureOwnership);
}

u32 ConvertColorToUint(const vec4 *Color) {
  // TODO: Check if this actually works
  i32 Red   = Min(Max(Color->r, 0.0f), 1.0f) * 255;
  i32 Green = Min(Max(Color->g, 0.0f), 1.0f) * 255;
  i32 Blue  = Min(Max(Color->b, 0.0f), 1.0f) * 255;
  i32 Alpha = Min(Max(Color->a, 0.0f), 1.0f) * 255;
  return (Alpha << 24) | (Blue << 16) | (Green << 8) | (Red << 0);
}

void Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, f32 Rotation, const vec4 *Color) {
  texture_specification Spec = {1, 1, IMAGE_FORMAT_RGBA8, false};
  texture2d *Tex = CreateTexture2D(&Spec);

  u32 ColorAsUint = ConvertColorToUint(Color);
  
  SetTexture2DData(Tex, (void *)&ColorAsUint, sizeof(ColorAsUint));
  
  Renderer2DDrawQuad(Pos, Size, Rotation, Tex, true);
}

void Renderer2DDrawQuad(const vec2 *Pos,
                        const vec2 *Size,
                        f32 Rotation,
                        texture2d *Tex,
                        bool TransferTextureOwnership) {
  vec3 Pos3 = { Pos->x, Pos->y, 0.0f };
  vec3 Size3 = { Size->x, Size->y, 1.0f };
  mat4 Translate = TranslationMatrix(&Pos3);
  mat4 Rotate = RotationMatrix2D(Rotation);
  mat4 Scale = ScaleMatrix(&Size3);
  mat4 Transform = Matmul(&Translate, &Rotate, &Scale);

  Renderer2DDrawQuad(&Transform, Tex, TransferTextureOwnership);
}

void Renderer2DDrawQuad(const mat4 *Transform, texture2d *Tex, bool TransferTextureOwnership) {
  if (Data.TextureSlotIndex == Data.MaxTextureSlots ||
      Data.QuadIndexCount == Data.MaxQuadVertices) {
    Renderer2DNextBatch();
  }

  f32 Slot = -1;
  for (u32 i = 0; i < Data.TextureSlotIndex; i++) {
    if (CompareTexture2D(Data.TextureSlots[i], Tex)) {
      Slot = i;
      break;
    }
  }
  if (Slot == -1) {
    Slot = Data.TextureSlotIndex;
    Data.TextureSlots[(i32)Slot] = Tex;
    Data.TextureSlotIndex++;
  }

  // Store Texture in Renderer2DData
  if (TransferTextureOwnership) {
    i32 RendererSlot = -1;
    for (u32 i = 0; i < Data.RendererTextureCount; i++) {
      if (CompareTexture2D(Data.RendererTextures[i], Tex)) {
        Slot = i;
        break;
      }
    }
    if (RendererSlot == -1) {
      Data.RendererTextures[Data.RendererTextureCount] = Tex;
    }
  }

  for (u32 i = 0; i < Data.VerticesPerQuad; i++) {
    vec4 Temp = { Data.QuadVertexPositions[i].x,
                  Data.QuadVertexPositions[i].y,
                  Data.QuadVertexPositions[i].z, 
                  1.0f };
    vec4 NewPos = MatVecMult(Transform, &Temp);
    Data.QuadVertexCurrent->Pos = { NewPos.x, NewPos.y, NewPos.z };
    Data.QuadVertexCurrent->TexCoord = Data.QuadTexCoords[i];
    Data.QuadVertexCurrent->TexIndex = Slot;
    Data.QuadVertexCurrent++;
  }

  Data.QuadIndexCount += Data.IndicesPerQuad;
}

void Renderer2DBeginScene(const scene_view_camera *Cam) {
  Data.CameraData.ViewProj = Cam->ViewProj;
  SetUniformBufferData(Data.CameraUniformBuffer, &Data.CameraData.ViewProj, sizeof(camera_data));
  Renderer2DStartBatch();
}

void Renderer2DEndScene() {
  Renderer2DFlush();
}
