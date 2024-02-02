#include "Renderer2D.h"

#include "PlatformManager.h"
#include "Log.h"

namespace SK
{
    Renderer2DData Renderer2D::sData;

    void Renderer2D::Init()
    {
        // QUADS
        sData.quadVertexArray = PlatformManager::NewVertexArray();
        sData.quadVertexBuffer = PlatformManager::NewVertexBuffer(sData.maxVertices * sizeof(QuadVertex));
        sData.quadVertexBuffer->SetLayout({
                                          { ShaderDataType::FLOAT3, "aPosition", false },
                                          { ShaderDataType::FLOAT4, "aColor", false },
                                          { ShaderDataType::FLOAT2, "aTextureCoordinate", false },
                                          { ShaderDataType::FLOAT, "aTextureIndex", false },
                                          { ShaderDataType::FLOAT, "aTilingFactor", false }
                                          });
        sData.quadVertexArray->AddVertexBuffer(sData.quadVertexBuffer);
        sData.quadVertexBufferBase = new QuadVertex[sData.maxVertices];
        u32* quadIndices = new u32[sData.maxIndices];
        u32 offset = 0;
        for(u32 i = 0; i < sData.maxIndices; i += INDICES_PER_QUAD)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += VERTICES_PER_QUAD;
        }
        Shared<IndexBuffer> quadIndexBuffer = PlatformManager::NewIndexBuffer(quadIndices, sData.maxIndices);
        sData.quadVertexArray->SetIndexBuffer(quadIndexBuffer);
        delete[] quadIndices;

        // CIRCLES
        sData.circleVertexArray = PlatformManager::NewVertexArray();
        sData.circleVertexBuffer = PlatformManager::NewVertexBuffer(sData.maxVertices * sizeof(CircleVertex));
        sData.circleVertexBuffer->SetLayout({
                                            { ShaderDataType::FLOAT3, "aWorldPosition", false },
                                            { ShaderDataType::FLOAT3, "aLocalPosition", false },
                                            { ShaderDataType::FLOAT4, "aColor", false },
                                            { ShaderDataType::FLOAT, "aThickness", false },
                                            { ShaderDataType::FLOAT, "aFade", false }
                                            });
        sData.circleVertexArray->AddVertexBuffer(sData.circleVertexBuffer);
        sData.circleVertexArray->SetIndexBuffer(quadIndexBuffer);
        sData.circleVertexBufferBase = new CircleVertex[sData.maxVertices];

        // LINES
        sData.lineVertexArray = PlatformManager::NewVertexArray();
        sData.lineVertexBuffer = PlatformManager::NewVertexBuffer(sData.maxVertices * sizeof(LineVertex));
        sData.lineVertexBuffer->SetLayout({
                                          { ShaderDataType::FLOAT3, "aPosition", false },
                                          { ShaderDataType::FLOAT4, "aColor", false }
                                          });
        sData.lineVertexArray->AddVertexBuffer(sData.lineVertexBuffer);
        sData.lineVertexBufferBase = new LineVertex[sData.maxVertices];
        sData.lineWidth = 2.0f;

        // TEXT
        sData.textVertexArray = PlatformManager::NewVertexArray();
        sData.textVertexBuffer = PlatformManager::NewVertexBuffer(sData.maxVertices * sizeof(TextVertex));
        sData.textVertexBuffer->SetLayout({
                                          { ShaderDataType::FLOAT3, "aPosition", false },
                                          { ShaderDataType::FLOAT4, "aColor", false },
                                          { ShaderDataType::FLOAT2, "aTextureCoordinate", false }
                                          });
        sData.textVertexArray->AddVertexBuffer(sData.textVertexBuffer);
        sData.textVertexArray->SetIndexBuffer(quadIndexBuffer);
        sData.textVertexBufferBase = new TextVertex[sData.maxVertices];

        sData.whiteTexture = PlatformManager::NewTexture2D(TextureSpecification());
        u32 whiteTextureData = 0xffffffff;
        sData.whiteTexture->SetData(&whiteTextureData, sizeof(u32));

        i32 samplers[sData.maxTextureSlots];
        for(u32 i = 0; i < sData.maxTextureSlots; i++)
        {
            samplers[i] = i;
        }

        sData.quadShader = PlatformManager::NewShader("Quad2DVertex.glsl", "Quad2DFragment.glsl");
        sData.circleShader = PlatformManager::NewShader("Circle2DVertex.glsl", "Circle2DFragment.glsl");
        sData.lineShader = PlatformManager::NewShader("Line2DVertex.glsl", "Line2DFragment.glsl");
        sData.textShader = PlatformManager::NewShader("Text2DVertex.glsl", "Text2DFragment.glsl");

        sData.textureSlots[0] = sData.whiteTexture;
        sData.textureSlotIndex = 1;

        sData.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        sData.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        sData.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        sData.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
        //sData.cameraUniformBuffer = PlatformManager::NewUniformBuffer(sizeof(Renderer2DData::CameraData), 0);
    }

    void Renderer2D::Shutdown()
    {
        sData.quadShader.reset();
        sData.circleShader.reset();
        sData.lineShader.reset();
        sData.textShader.reset();

        sData.whiteTexture.reset();

        sData.quadVertexArray.reset();
        sData.quadVertexBuffer.reset();

        sData.circleVertexArray.reset();
        sData.circleVertexBuffer.reset();

        sData.lineVertexArray.reset();
        sData.lineVertexBuffer.reset();

        sData.textVertexArray.reset();
        sData.textVertexBuffer.reset();

        delete[] sData.quadVertexBufferBase;
        delete[] sData.circleVertexBufferBase;
        delete[] sData.lineVertexBufferBase;
        delete[] sData.textVertexBufferBase;
    }

    void Renderer2D::BeginScene(const Camera& camera)
    {
        // Start Camera
        sData.cameraBuffer.viewProj = camera.GetViewProjection();
        StartBatch();
    }

    void Renderer2D::BeginScene(const Camera& camera, const std::array<std::array<f32, 4>, 4>& position)
    {
        // Start Camera
        //sData.cameraBuffer.viewProj = camera.GetProjection() * Inverse(position);
        StartBatch();
        SK_CORE_WARN("Begin Scene With Position Parameter not implemented");
    }

    void Renderer2D::EndScene()
    {
        Flush();
    }

    void Renderer2D::DrawQuad()
    {

    }

    void Renderer2D::DrawCircle()
    {

    }

    void Renderer2D::DrawTriangle()
    {

    }

    void Renderer2D::DrawLine()
    {

    }

    void Renderer2D::DrawSprite()
    {

    }

    void Renderer2D::DrawString()
    {

    }

    void Renderer2D::Flush()
    {
        if(sData.quadIndexCount)
        {
            // These two lines tell the renderer how many vertices to bind and render in this pass
            u32 dataSize = (u32)((u8*)sData.quadVertexBufferPtr - (u8*)sData.quadVertexBufferBase); // Get Size of Quad Vertex Data using ptr math
            sData.quadVertexBuffer->SetData(sData.quadVertexBufferBase, dataSize); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

            // Bind All Textures
            for(u32 i = 0; i < sData.textureSlotIndex; i++)
            {
                sData.textureSlots[i]->Bind(i);
            }

            sData.quadShader->Bind();
            // Draw quadIndexCount quads fron quadVertexArray
            RenderCommand::DrawIndexed(sData.quadVertexArray, sData.quadIndexCount);
        }

        if(sData.circleIndexCount)
        {
            // These two lines tell the renderer how many vertices to bind and render in this pass
            u32 dataSize = (u32)((u8*)sData.circleVertexBufferPtr - (u8*)sData.circleVertexBufferBase); // Get Size of Quad Vertex Data using ptr math
            sData.circleVertexBuffer->SetData(sData.circleVertexBufferBase, dataSize); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

            // Bind All Textures
            for(u32 i = 0; i < sData.textureSlotIndex; i++)
            {
                sData.textureSlots[i]->Bind(i);
            }

            sData.circleShader->Bind();
            // Draw quadIndexCount quads fron quadVertexArray
            RenderCommand::DrawIndexed(sData.circleVertexArray, sData.circleIndexCount);
        }

        if(sData.lineVertexCount)
        {
            // These two lines tell the renderer how many vertices to bind and render in this pass
            u32 dataSize = (u32)((u8*)sData.lineVertexBufferPtr - (u8*)sData.lineVertexBufferBase); // Get Size of Quad Vertex Data using ptr math
            sData.lineVertexBuffer->SetData(sData.lineVertexBufferBase, dataSize); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

            // Bind All Textures
            for(u32 i = 0; i < sData.textureSlotIndex; i++)
            {
                sData.textureSlots[i]->Bind(i);
            }

            sData.lineShader->Bind();
            // Draw quadIndexCount quads fron quadVertexArray
            RenderCommand::SetLineWidth(sData.lineWidth);
            RenderCommand::DrawLines(sData.lineVertexArray, sData.lineVertexCount);
        }

        if(sData.textIndexCount)
        {
            // These two lines tell the renderer how many vertices to bind and render in this pass
            u32 dataSize = (u32)((u8*)sData.textVertexBufferPtr - (u8*)sData.textVertexBufferBase); // Get Size of Quad Vertex Data using ptr math
            sData.textVertexBuffer->SetData(sData.textVertexBufferBase, dataSize); // Set quad vertex buffer to base ptr and set data size to size of all quad vertices

            // Bind All Textures
            for(u32 i = 0; i < sData.textureSlotIndex; i++)
            {
                sData.textureSlots[i]->Bind(i);
            }

            sData.textShader->Bind();
            // Draw quadIndexCount quads fron quadVertexArray
            RenderCommand::DrawIndexed(sData.textVertexArray, sData.textIndexCount);
        }
    }

    void Renderer2D::StartBatch()
    {
        sData.quadIndexCount = 0;
        sData.quadVertexBufferPtr = sData.quadVertexBufferBase;

        sData.circleIndexCount = 0;
        sData.circleVertexBufferPtr = sData.circleVertexBufferBase;

        sData.lineVertexCount = 0;
        sData.lineVertexBufferPtr = sData.lineVertexBufferBase;

        sData.textIndexCount = 0;
        sData.textVertexBufferPtr = sData.textVertexBufferBase;

        sData.textureSlotIndex = 1;
    }

    void Renderer2D::Nextbatch()
    {
        Flush();
        StartBatch();
    }
}

