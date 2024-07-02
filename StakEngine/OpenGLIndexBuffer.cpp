#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "IndexBuffer.h"
#include "Log.h"

/*********************
 * Private Interface *
 *********************/

struct index_buffer
{
    u32 RendererId;
    u32 Count;
};

/********************
 * Public Interface *
 ********************/

index_buffer *
CreateIndexBuffer()
{
    index_buffer *IndexBuffer = (index_buffer *)malloc(sizeof(index_buffer));
    
    if (!IndexBuffer)
    {
        LogCoreError("Failed to allocate memory for index buffer");
        return NULL;
    }
    
    IndexBuffer->Count = 0;
    
    glGenBuffers(1, &IndexBuffer->RendererId);
    
    return IndexBuffer;
}

index_buffer *
CreateIndexBuffer(u32 *Indices, u32 Count)
{
    index_buffer *IndexBuffer = (index_buffer *)malloc(sizeof(index_buffer));
    
    if (!IndexBuffer) {
        LogCoreError("Failed to allocate memory for index buffer");
        return NULL;
    }

    IndexBuffer->Count = Count;
    
    glGenBuffers(1, &IndexBuffer->RendererId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer->RendererId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(u32), Indices, GL_STATIC_DRAW);
    
    return IndexBuffer;
}

void
DestroyIndexBuffer(index_buffer **IndexBuffer)
{
    if (*IndexBuffer != NULL) {
        glDeleteBuffers(1, &(*IndexBuffer)->RendererId);
        
        free(*IndexBuffer);
        
        *IndexBuffer = NULL;
    }
}

void
BindIndexBuffer(const index_buffer *IndexBuffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer->RendererId);
}

void
UnbindIndexBuffer(const index_buffer *IndexBuffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32
GetIndexBufferIndexCount(const index_buffer *IndexBuffer)
{
    return IndexBuffer->Count;
}

#endif
