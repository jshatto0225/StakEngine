#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "VertexBuffer.h"
#include "Log.h"

/*********************
 * Private Interface *
 *********************/

struct vertex_buffer
{
    u32 RendererId;
    buffer_layout *Layout;
};

/********************
 * Public Interface *
 ********************/

vertex_buffer *
CreateVertexBuffer(u32 Size)
{
    vertex_buffer *VertexBuffer = (vertex_buffer *)malloc(sizeof(vertex_buffer));

    if (!VertexBuffer)
    {
        LogCoreError("Failed to allocate memory for vertex buffer");
        return NULL;
    }

    glGenBuffers(1, &VertexBuffer->RendererId);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
    glBufferData(GL_ARRAY_BUFFER, Size, NULL, GL_DYNAMIC_DRAW);
    
    VertexBuffer->Layout = NULL;

    return VertexBuffer;
}

vertex_buffer *
CreateVertexBuffer(const f32 *Vertices, u32 Size)
{
    vertex_buffer *VertexBuffer = (vertex_buffer *)malloc(sizeof(vertex_buffer));
    
    if (!VertexBuffer)
    {
        LogCoreError("Failed to allocate memory for vertex buffer");
        return NULL;
    }
    
    glGenBuffers(1, &VertexBuffer->RendererId);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
    glBufferData(GL_ARRAY_BUFFER, Size, Vertices, GL_DYNAMIC_DRAW);
    
    VertexBuffer->Layout = NULL;
    
    return VertexBuffer;
}

void
DestroyVertexBuffer(vertex_buffer **VertexBuffer)
{
    if (*VertexBuffer != NULL) {
        glDeleteBuffers(1, &(*VertexBuffer)->RendererId);
        
        DestroyBufferLayout(&(*VertexBuffer)->Layout);
        
        free(*VertexBuffer);
        
        *VertexBuffer = NULL;
    }
}

void
BindVertexBuffer(const vertex_buffer *VertexBuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
}

void
UnbindVertexBuffer(const vertex_buffer *Vertexbuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
SetVertexBufferLayout(vertex_buffer *VertexBuffer, buffer_layout **BufferLayout)
{
    VertexBuffer->Layout = *BufferLayout;
    *BufferLayout = NULL;
}

void
SetVertexBufferData(vertex_buffer *VertexBuffer, const void *Data, u32 Size)
{
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Size, Data);
}

const buffer_layout *
GetVertexBufferLayout(const vertex_buffer *VertexBuffer)
{
    return VertexBuffer->Layout;
}

vertex_buffer_array *
CreateVertexBufferArray(u64 Capacity)
{
    vertex_buffer_array *Array = (vertex_buffer_array *)malloc(sizeof(vertex_buffer_array));
    
    if (!Array)
    {
        LogCoreError("Failed to allocate memory for vertex buffer array");
        return NULL;
    }
    
    Array->Capacity = Capacity;
    Array->Size = 0;
    Array->Buffers = (const vertex_buffer **)malloc(sizeof(vertex_buffer *) * Array->Capacity);
    
    return Array;
}

void
AddVertexBufferToArray(vertex_buffer_array *Array, const vertex_buffer *VertexBuffer)
{
    if (Array->Size == Array->Capacity)
    {
        Array->Capacity *= 2;
        const vertex_buffer **Res = (const vertex_buffer **)realloc((void *)Array->Buffers, sizeof(vertex_buffer *) * Array->Capacity);
      
        if (!Res)
        {
            LogCoreError("Failed to reallocate vertex buffer array");
            return;
        }
    }
    
    Array->Buffers[Array->Size] = VertexBuffer;
    Array->Size++;
}

void
DestroyVertexBufferArray(vertex_buffer_array **Array)
{
    if (*Array != NULL)
    {
        free(*Array);
        
        *Array = NULL;
    }
}

#endif
