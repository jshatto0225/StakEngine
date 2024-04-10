#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "UniformBuffer.h"
#include "Log.h"

/*********************
 * Private Interface *
 *********************/

struct uniform_buffer
{
    u32 RendererId;
};

/********************
 * Public Interface *
 ********************/

uniform_buffer *
CreateUniformBuffer(u32 Size, u32 Binding)
{
    uniform_buffer *UniformBuffer = (uniform_buffer *)malloc(sizeof(uniform_buffer));

    if (!UniformBuffer)
    {
        LogCoreError("Failed to allocate memory for uniform buffer");
        return NULL;
    }

    glCreateBuffers(1, &UniformBuffer->RendererId);
    glNamedBufferData(UniformBuffer->RendererId, Size, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, Binding, UniformBuffer->RendererId);
    
    return UniformBuffer;
}

void
DestroyUniformBuffer(uniform_buffer **UniformBuffer)
{
    if (*UniformBuffer != NULL)
    {
        glDeleteBuffers(1, &(*UniformBuffer)->RendererId);
        
        free(*UniformBuffer);
        
        *UniformBuffer = NULL;
    }
}

void
SetUniformBufferData(uniform_buffer *UniformBuffer, const void *Data, u32 Size, u32 Offset)
{
    glNamedBufferSubData(UniformBuffer->RendererId, Offset, Size, Data);
}

#endif
