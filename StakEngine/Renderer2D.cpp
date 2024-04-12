#include "Renderer2D.h"

#include "Platform.h"
#include "Log.h"
#include "UniformBuffer.h"
#include "RenderCommand.h"

///////////////////////
// Private Interface //
///////////////////////

struct quad_vertex
{
    vec3 Pos;
    vec2 TexCoord;
    f32 TexIndex;
};

struct circle_vertex
{
    vec3 Pos;
    vec2 TexCoord;
    f32 TexIndex;
};

struct line_vertex
{
    // TODO:
};

struct text_vertex
{
    // TODO:
};

struct camera_data
{
    mat4 ViewProj;
};

static constexpr u32 MAX_QUADS = 1000;
static constexpr u32 VERTICES_PER_QUAD = 4;
static constexpr u32 INDICES_PER_QUAD = 6;
static constexpr u32 MAX_QUAD_VERTICES = MAX_QUADS * VERTICES_PER_QUAD;
static constexpr u32 MAX_QUAD_INDICES = MAX_QUADS * INDICES_PER_QUAD;

static constexpr u32 MAX_CIRCLES = MAX_QUADS;
static constexpr u32 VERTICES_PER_CIRCLE = VERTICES_PER_QUAD;
static constexpr u32 INDICES_PER_CIRCLE = INDICES_PER_QUAD;
static constexpr u32 MAX_CIRCLE_VERTICES = MAX_QUAD_VERTICES;
static constexpr u32 MAX_CIRCLE_INDICES = MAX_QUAD_INDICES;

static constexpr u32 MAX_TEXTURE_SLOTS = 32;

struct renderer2d_data
{
    quad_vertex *QuadVerticesBase;
    quad_vertex *QuadVertexCurrent;
    u32 QuadIndexCount;

    circle_vertex *CircleVerticesBase;
    circle_vertex *CircleVertexCurrent;
    u32 CircleIndexCount;

    vertex_buffer *QuadVertexBuffer;
    vertex_array *QuadVertexArray;
    shader *QuadShader;

    vertex_buffer *CircleVertexBuffer;
    vertex_array *CircleVertexArray;
    shader *CircleShader;

    texture2d *WhiteTexture;
    const texture2d *TextureSlots[MAX_TEXTURE_SLOTS];
    texture2d *RendererTextures[MAX_TEXTURE_SLOTS];
    u32 RendererTextureCount;

    u32 TextureSlotIndex;

    camera_data CameraData;
    uniform_buffer *CameraUniformBuffer;

    vec3 QuadVertexPositions[4];
    vec2 QuadTexCoords[4];

    vec3 CircleVertexPositions[4];
    vec2 CircleTexCoords[4];
};

static renderer2d_data Data;

//////////////////////
// Public Interface //
//////////////////////

void
Renderer2DFlush()
{
    if (Data.QuadIndexCount)
    {
        u32 DataSize = (u32)((u8 *)Data.QuadVertexCurrent - (u8 *)Data.QuadVerticesBase);
        SetVertexBufferData(Data.QuadVertexBuffer, Data.QuadVerticesBase, DataSize);

        for (u32 i = 0; i < Data.TextureSlotIndex; i++)
        {
            BindTexture2D(Data.TextureSlots[i], i);
        }

        BindVertexArray(Data.QuadVertexArray);

        BindShader(Data.QuadShader);

        RenderCommandDrawIndexed(Data.QuadVertexArray, Data.QuadIndexCount);
    }
    if (Data.CircleIndexCount)
    {
        u32 DataSize = (u32)((u8 *)Data.CircleVertexCurrent - (u8 *)Data.CircleVerticesBase);
        SetVertexBufferData(Data.CircleVertexBuffer, Data.CircleVerticesBase, DataSize);

        for (u32 i = 0; i < Data.TextureSlotIndex; i++)
        {
            BindTexture2D(Data.TextureSlots[i], i);
        }

        BindVertexArray(Data.CircleVertexArray);

        BindShader(Data.CircleShader);

        RenderCommandDrawIndexed(Data.CircleVertexArray, Data.CircleIndexCount);
    }
}

void
Renderer2DStartBatch()
{
    Data.QuadIndexCount = 0;
    Data.QuadVertexCurrent = Data.QuadVerticesBase;

    Data.CircleIndexCount = 0;
    Data.CircleVertexCurrent = Data.CircleVerticesBase;

    for (u32 i = 0; i < Data.RendererTextureCount; i++)
    {
        DestroyTexture2D(&Data.RendererTextures[i]);
    }

    Data.RendererTextureCount = 0;
    Data.TextureSlotIndex = 1;
}

void
Renderer2DNextBatch()
{
    Renderer2DFlush();
    Renderer2DStartBatch();
}

void
Renderer2DInit()
{
    u32 *QuadIndices = (u32 *)malloc(MAX_QUAD_INDICES * sizeof(u32));
    u32 *CircleIndices = (u32 *)malloc(MAX_CIRCLE_INDICES * sizeof(u32));

    if (!QuadIndices)
    {
        LogCoreError("Failed to allocate memory for quad incices");
        return;
    }
    if (!CircleIndices)
    {
        LogCoreError("Failed to allocate memory for circle indices");
        return;
    }

    u32 Offset = 0;
    for (u32 i = 0; i < MAX_QUAD_INDICES; i += INDICES_PER_QUAD)
    {
        QuadIndices[i + 0] = Offset + 0;
        QuadIndices[i + 1] = Offset + 1;
        QuadIndices[i + 2] = Offset + 3;
        QuadIndices[i + 3] = Offset + 1;
        QuadIndices[i + 4] = Offset + 2;
        QuadIndices[i + 5] = Offset + 3;

        CircleIndices[i + 0] = Offset + 0;
        CircleIndices[i + 1] = Offset + 1;
        CircleIndices[i + 2] = Offset + 3;
        CircleIndices[i + 3] = Offset + 1;
        CircleIndices[i + 4] = Offset + 2;
        CircleIndices[i + 5] = Offset + 3;

        Offset += VERTICES_PER_QUAD;
    }

    Data.QuadIndexCount = 0;
    Data.QuadVerticesBase = (quad_vertex *)malloc(MAX_QUAD_INDICES * sizeof(quad_vertex));
    Data.QuadVertexCurrent = Data.QuadVerticesBase;

    Data.CircleIndexCount = 0;
    Data.CircleVerticesBase = (circle_vertex *)malloc(MAX_CIRCLE_INDICES * sizeof(circle_vertex));
    Data.CircleVertexCurrent = Data.CircleVerticesBase;

    if (!Data.QuadVerticesBase)
    {
        LogCoreError("Failed to allocate memory for quad vertices");
        return;
    }
    if (!Data.CircleVerticesBase)
    {
        LogCoreError("Failed to allocate memory for circle vertices");
        return;
    }

    Data.QuadShader = CreateShader(SHADER_DIR "QuadShader.glsl");
    Data.CircleShader = CreateShader(SHADER_DIR "CircleShader.glsl");

    Data.QuadVertexBuffer = CreateVertexBuffer((u32)sizeof(quad_vertex) * MAX_QUAD_VERTICES);
    buffer_layout *QuadBufferLayout = CreateBufferLayout(3);
    AddElementToLayout(QuadBufferLayout, 0, SDT_FLOAT3, "aPosition", false);
    AddElementToLayout(QuadBufferLayout, 1, SDT_FLOAT2, "aTexCoord", false);
    AddElementToLayout(QuadBufferLayout, 2, SDT_FLOAT,  "aTexIndex", false);
    CalculateOffsetsAndStride(QuadBufferLayout);
    SetVertexBufferLayout(Data.QuadVertexBuffer, &QuadBufferLayout);

    Data.CircleVertexBuffer = CreateVertexBuffer((u32)sizeof(circle_vertex) * MAX_CIRCLE_VERTICES);
    buffer_layout *CircleBufferLayout = CreateBufferLayout(3);
    AddElementToLayout(CircleBufferLayout, 0, SDT_FLOAT3, "aPosition", false);
    AddElementToLayout(CircleBufferLayout, 1, SDT_FLOAT2, "aTexCoord", false);
    AddElementToLayout(CircleBufferLayout, 2, SDT_FLOAT, "aTexIndex", false);
    CalculateOffsetsAndStride(CircleBufferLayout);
    SetVertexBufferLayout(Data.CircleVertexBuffer, &CircleBufferLayout);

    index_buffer *QuadIndexBuffer = CreateIndexBuffer(QuadIndices, MAX_QUAD_INDICES);
    index_buffer *CircleIndexBuffer = CreateIndexBuffer(CircleIndices, MAX_CIRCLE_INDICES);

    free(QuadIndices);
    free(CircleIndices);

    Data.QuadVertexArray = CreateVertexArray();
    AddVertexBufferToVertexArray(Data.QuadVertexArray, Data.QuadVertexBuffer);
    SetVertexArrayIndexBuffer(Data.QuadVertexArray, &QuadIndexBuffer);

    Data.CircleVertexArray = CreateVertexArray();
    AddVertexBufferToVertexArray(Data.CircleVertexArray, Data.CircleVertexBuffer);
    SetVertexArrayIndexBuffer(Data.CircleVertexArray, &CircleIndexBuffer);

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

    Data.CircleVertexPositions[0] = {  0.5f,  0.5f, 0.0f };
    Data.CircleVertexPositions[1] = {  0.5f, -0.5f, 0.0f };
    Data.CircleVertexPositions[2] = { -0.5f, -0.5f, 0.0f };
    Data.CircleVertexPositions[3] = { -0.5f,  0.5f, 0.0f };

    Data.QuadTexCoords[0] = { 1.0f, 1.0f };
    Data.QuadTexCoords[1] = { 1.0f, 0.0f };
    Data.QuadTexCoords[2] = { 0.0f, 0.0f };
    Data.QuadTexCoords[3] = { 0.0f, 1.0f };

    Data.CircleTexCoords[0] = { 1.0f, 1.0f };
    Data.CircleTexCoords[1] = { 1.0f, 0.0f };
    Data.CircleTexCoords[2] = { 0.0f, 0.0f };
    Data.CircleTexCoords[3] = { 0.0f, 1.0f };
}

void
Renderer2DShutdown()
{
    DestroyTexture2D(&Data.WhiteTexture);

    free(Data.QuadVerticesBase);
    DestroyShader(&Data.QuadShader);
    DestroyVertexBuffer(&Data.QuadVertexBuffer);
    DestroyVertexArray(&Data.QuadVertexArray);

    free(Data.CircleVerticesBase);
    DestroyShader(&Data.CircleShader);
    DestroyVertexBuffer(&Data.CircleVertexBuffer);
    DestroyVertexArray(&Data.CircleVertexArray);

    DestroyUniformBuffer(&Data.CameraUniformBuffer);

    for (u32 i = 0; i < Data.RendererTextureCount; i++)
    {
        DestroyTexture2D(&Data.RendererTextures[i]);
    }
}

u32
ConvertColorToUint(const vec4 *Color)
{
    // TODO: Check if this actually works
    i32 Red   = (i32)(Min(Max(Color->r, 0.0f), 1.0f) * 255.0f);
    i32 Green = (i32)(Min(Max(Color->g, 0.0f), 1.0f) * 255.0f);
    i32 Blue  = (i32)(Min(Max(Color->b, 0.0f), 1.0f) * 255.0f);
    i32 Alpha = (i32)(Min(Max(Color->a, 0.0f), 1.0f) * 255.0f);
    return (Alpha << 24) | (Blue << 16) | (Green << 8) | (Red << 0);
}

void
Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, const vec4 *Color)
{
    Renderer2DDrawCircle(Pos, Size, 0, Color);
}

void
Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, texture2d **Tex, bool TransferTextureOwnership)
{
    Renderer2DDrawCircle(Pos, Size, 0, Tex, TransferTextureOwnership);
}

void
Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, f32 Rotation, const vec4 *Color)
{
    texture_specification Spec = {1, 1, IMAGE_FORMAT_RGBA8, false};
    texture2d *Tex = CreateTexture2D(&Spec);

    u32 ColorAsUint = ConvertColorToUint(Color);

    SetTexture2DData(Tex, (void *)&ColorAsUint, sizeof(ColorAsUint));

    Renderer2DDrawCircle(Pos, Size, Rotation, &Tex, true);
}

void
Renderer2DDrawCircle(const vec2 *Pos, const vec2 *Size, f32 Rotation, texture2d **Tex, bool TransferTextureOwnership)
{
    vec3 Pos3 = { Pos->x, Pos->y, 0.0f };
    vec3 Size3 = { Size->x, Size->y, 1.0f };
    mat4 Translate = TranslationMatrix(&Pos3);
    mat4 Rotate = RotationMatrix2D(Rotation);
    mat4 Scale = ScaleMatrix(&Size3);
    mat4 Transform = Matmul(&Translate, &Rotate, &Scale);

    Renderer2DDrawCircle(&Transform, Tex, TransferTextureOwnership);
}

void
Renderer2DDrawCircle(const mat4 *Transform, texture2d **Tex, bool TransferTextureOwnership)
{
    if (Data.TextureSlotIndex == MAX_TEXTURE_SLOTS || Data.CircleIndexCount == MAX_CIRCLE_VERTICES)
    {
        Renderer2DNextBatch();
    }

    f32 Slot = -1;
    for (u32 i = 0; i < Data.TextureSlotIndex; i++)
    {
        if (CompareTexture2D(Data.TextureSlots[i], *Tex))
        {
            Slot = (f32)i;
            break;
        }
    }
    if (Slot == -1)
    {
        Slot = (f32)Data.TextureSlotIndex;
        Data.TextureSlots[(i32)Slot] = *Tex;
        Data.TextureSlotIndex++;
    }

    // Store Texture in Renderer2DData
    if (TransferTextureOwnership)
    {
        i32 RendererSlot = -1;
        for (u32 i = 0; i < Data.RendererTextureCount; i++)
        {
            if (CompareTexture2D(Data.RendererTextures[i], *Tex))
            {
                Slot = (f32)i;
                break;
            }
        }
        if (RendererSlot == -1)
        {
            Data.RendererTextures[Data.RendererTextureCount] = *Tex;
            Data.RendererTextureCount++;
            *Tex = NULL;
        }
        else
        {
            // Destroy it if it already exists
            // Probably should not happen
            DestroyTexture2D(Tex);
        }
    }

    for (u32 i = 0; i < VERTICES_PER_CIRCLE; i++)
    {
        vec4 Temp =
            {
                Data.CircleVertexPositions[i].x,
                Data.CircleVertexPositions[i].y,
                Data.CircleVertexPositions[i].z,
                1.0f
            };
        vec4 NewPos = MatVecMult(Transform, &Temp);
        Data.CircleVertexCurrent->Pos = { NewPos.x, NewPos.y, NewPos.z };
        Data.CircleVertexCurrent->TexCoord = Data.CircleTexCoords[i];
        Data.CircleVertexCurrent->TexIndex = Slot;
        Data.CircleVertexCurrent++;
    }

    Data.CircleIndexCount += INDICES_PER_CIRCLE;
}

void
Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, const vec4 *Color)
{
    Renderer2DDrawQuad(Pos, Size, 0, Color);
}

void
Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, texture2d **Tex, bool TransferTextureOwnership)
{
    Renderer2DDrawQuad(Pos, Size, 0, Tex, TransferTextureOwnership);
}

void
Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, f32 Rotation, const vec4 *Color)
{
    texture_specification Spec = {1, 1, IMAGE_FORMAT_RGBA8, false};
    texture2d *Tex = CreateTexture2D(&Spec);

    u32 ColorAsUint = ConvertColorToUint(Color);

    SetTexture2DData(Tex, (void *)&ColorAsUint, sizeof(ColorAsUint));

    Renderer2DDrawQuad(Pos, Size, Rotation, &Tex, true);
}

void
Renderer2DDrawQuad(const vec2 *Pos, const vec2 *Size, f32 Rotation, texture2d **Tex, bool TransferTextureOwnership)
{
    vec3 Pos3 = { Pos->x, Pos->y, 0.0f };
    vec3 Size3 = { Size->x, Size->y, 1.0f };
    mat4 Translate = TranslationMatrix(&Pos3);
    mat4 Rotate = RotationMatrix2D(Rotation);
    mat4 Scale = ScaleMatrix(&Size3);
    mat4 Transform = Matmul(&Translate, &Rotate, &Scale);

    Renderer2DDrawQuad(&Transform, Tex, TransferTextureOwnership);
}

void
Renderer2DDrawQuad(const mat4 *Transform, texture2d **Tex, bool TransferTextureOwnership)
{
    if (Data.TextureSlotIndex == MAX_TEXTURE_SLOTS || Data.QuadIndexCount == MAX_QUAD_VERTICES)
    {
        Renderer2DNextBatch();
    }

    f32 Slot = -1;
    for (u32 i = 0; i < Data.TextureSlotIndex; i++)
    {
        if (CompareTexture2D(Data.TextureSlots[i], *Tex))
        {
            Slot = (f32)i;
            break;
        }
    }
    if (Slot == -1)
    {
        Slot = (f32)Data.TextureSlotIndex;
        Data.TextureSlots[(i32)Slot] = *Tex;
        Data.TextureSlotIndex++;
    }

    // Store Texture in Renderer2DData
    if (TransferTextureOwnership)
    {
        i32 RendererSlot = -1;
        for (u32 i = 0; i < Data.RendererTextureCount; i++)
        {
            if (CompareTexture2D(Data.RendererTextures[i], *Tex))
            {
                Slot = (f32)i;
                break;
            }
        }
        if (RendererSlot == -1)
        {
            Data.RendererTextures[Data.RendererTextureCount] = *Tex;
            Data.RendererTextureCount++;
            *Tex = NULL;
        }
        else
        {
            // Destroy it if it already exists
            // Probably should not happen
            DestroyTexture2D(Tex);
        }
    }

    for (u32 i = 0; i < VERTICES_PER_QUAD; i++)
    {
        vec4 Temp =
            {
                Data.QuadVertexPositions[i].x,
                Data.QuadVertexPositions[i].y,
                Data.QuadVertexPositions[i].z,
                1.0f
            };
        vec4 NewPos = MatVecMult(Transform, &Temp);
        Data.QuadVertexCurrent->Pos = { NewPos.x, NewPos.y, NewPos.z };
        Data.QuadVertexCurrent->TexCoord = Data.QuadTexCoords[i];
        Data.QuadVertexCurrent->TexIndex = Slot;
        Data.QuadVertexCurrent++;
    }

    Data.QuadIndexCount += INDICES_PER_QUAD;
}

void
Renderer2DBeginScene(const scene_view_camera *Cam)
{
    Data.CameraData.ViewProj = Cam->ViewProj;
    SetUniformBufferData(Data.CameraUniformBuffer, &Data.CameraData.ViewProj, sizeof(camera_data));
    Renderer2DStartBatch();
}

void
Renderer2DEndScene()
{
    Renderer2DFlush();
}
