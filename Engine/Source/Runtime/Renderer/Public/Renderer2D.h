#pragma once

#include <queue>

#include "RenderCommand.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#define MAX_QUADS 1000
#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define MAX_TEXTURE_SLOTS 32

struct QuadVertex
{
    std::array<f32, 3> position;
    std::array<f32, 4> color;
    std::array<f32, 2> textureCoordinate;
    f32 textureIndex;
    f32 tilingFactor;
};

struct CircleVertex
{
    std::array<f32, 3> worldPosition;
    std::array<f32, 3> locaPosition;
    std::array<f32, 4> color;
    f32 thickness;
    f32 fade;
};

struct LineVertex
{
    std::array<f32, 3> position;
    std::array<f32, 4> color;
};

struct TextVertex
{
    std::array<f32, 3> position;
    std::array<f32, 4> color;
    std::array<f32, 2> textureCoordinate;
};

struct Renderer2DData
{
    static const u32 maxQuads = MAX_QUADS;
    static const u32 maxVertices = maxQuads * VERTICES_PER_QUAD;
    static const u32 maxIndices = maxQuads * INDICES_PER_QUAD;
    static const u32 maxTextureSlots = MAX_TEXTURE_SLOTS;

    Shared<VertexArray> quadVertexArray;
    Shared<VertexBuffer> quadVertexBuffer;
    Shared<Shader> quadShader;

    Shared<VertexArray> circleVertexArray;
    Shared<VertexBuffer> circleVertexBuffer;
    Shared<Shader> circleShader;

    Shared<VertexArray> lineVertexArray;
    Shared<VertexBuffer> lineVertexBuffer;
    Shared<Shader> lineShader;

    Shared<VertexArray> textVertexArray;
    Shared<VertexBuffer> textVertexBuffer;
    Shared<Shader> textShader;

    QuadVertex* quadVertexBufferBase;
    QuadVertex* quadVertexBufferPtr;
    u32 quadIndexCount;

    CircleVertex* circleVertexBufferBase;
    CircleVertex* circleVertexBufferPtr;
    u32 circleIndexCount;

    LineVertex* lineVertexBufferBase;
    LineVertex* lineVertexBufferPtr;
    u32 lineVertexCount;

    TextVertex* textVertexBufferBase;
    TextVertex* textVertexBufferPtr;
    u32 textIndexCount;

    Shared<Texture2D> whiteTexture;

    f32 lineWidth;

    std::array<Shared<Texture2D>, maxTextureSlots> textureSlots;
    u32 textureSlotIndex;

    std::array<std::array<f32, 4>, 4> quadVertexPositions;

    struct CameraBuffer
    {
        std::array<std::array<f32, 4>, 4> viewProj;
    };
    CameraBuffer cameraBuffer;
};

class Renderer2D final
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const Camera& camera);
    static void BeginScene(const Camera& camera, const std::array<std::array<f32, 4>, 4>& position);
    static void EndScene();

    static void DrawQuad();
    static void DrawCircle();
    static void DrawTriangle();
    static void DrawLine();
    static void DrawSprite();
    static void DrawString();

    static void Flush();
    static void StartBatch();
    static void Nextbatch();

private:
    static Renderer2DData sData;
};
