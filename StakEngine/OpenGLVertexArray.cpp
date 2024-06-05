#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Log.h"

/*********************
 * Private Interface *
 *********************/

struct vertex_array
{
    vertex_buffer_array *VertexBuffers;
    index_buffer *IndexBuffer;
    u32 RendererId;
    u32 VertexBufferIndex;
};

 /********************
  * Public Interface *
  ********************/

vertex_array *
CreateVertexArray()
{
    vertex_array *VertexArray = (vertex_array *)malloc(sizeof(vertex_array));
    
    if (!VertexArray)
    {
        LogCoreError("Failed to allocate memory for vertex array");
        return NULL;
    }
    
    VertexArray->VertexBuffers = CreateVertexBufferArray(16);
    
    glGenVertexArrays(1, &VertexArray->RendererId);
    VertexArray->VertexBufferIndex = 0;
    
    VertexArray->IndexBuffer = NULL;
    
    return VertexArray;
}

void
DestroyVertexArray(vertex_array **VertexArray)
{
    if (*VertexArray != NULL)
    {
        glDeleteVertexArrays(1, &(*VertexArray)->RendererId);
        
        DestroyIndexBuffer(&(*VertexArray)->IndexBuffer);
        DestroyVertexBufferArray(&(*VertexArray)->VertexBuffers);
        
        free(*VertexArray);
        
        *VertexArray = NULL;
    }
}

void
AddVertexBufferToVertexArray(vertex_array *VertexArray, const vertex_buffer *VertexBuffer)
{
    BindVertexArray(VertexArray);
    BindVertexBuffer(VertexBuffer);
    const buffer_layout *Layout = GetVertexBufferLayout(VertexBuffer);
    for (u64 i = 0; i < Layout->Size; i++)
    {
        buffer_element Element = Layout->Elements[i];
        switch (Element.Type)
        {
        case SDT_INT:
        case SDT_INT2:
        case SDT_INT3:
        case SDT_INT4:
        case SDT_FLOAT:
        case SDT_FLOAT2:
        case SDT_FLOAT3:
        case SDT_FLOAT4:
        {
            glEnableVertexAttribArray(VertexArray->VertexBufferIndex);
            glVertexAttribPointer(VertexArray->VertexBufferIndex, GetBufferElementComponentCount(&Element), ShaderDataTypeToOpenGLType(Element.Type), Element.Normalized ? GL_TRUE : GL_FALSE, Layout->Stride, (const void *)Element.Offset);
            VertexArray->VertexBufferIndex++;
            break;
        }
        case SDT_BOOL:
        {
            glEnableVertexAttribArray(VertexArray->VertexBufferIndex);
            glVertexAttribPointer(VertexArray->VertexBufferIndex, GetBufferElementComponentCount(&Element), ShaderDataTypeToOpenGLType(Element.Type), Element.Normalized ? GL_TRUE : GL_FALSE, Layout->Stride, (const void *)Element.Offset);
            VertexArray->VertexBufferIndex++;
            break;
        }
        case SDT_MAT3:
        case SDT_MAT4:
        {
            u8 Count = GetBufferElementComponentCount(&Element);
            for (u8 i = 0; i < Count; i++) {
                glEnableVertexAttribArray(VertexArray->VertexBufferIndex);
                glVertexAttribPointer(VertexArray->VertexBufferIndex, Count, ShaderDataTypeSize(Element.Type), Element.Normalized ? GL_TRUE : GL_FALSE, Layout->Stride, (const void *)(Element.Offset + sizeof(f32) * Count * i));
                glVertexAttribDivisor(VertexArray->VertexBufferIndex, 1);
                VertexArray->VertexBufferIndex++;
            }
        }
        }
    }
    AddVertexBufferToArray(VertexArray->VertexBuffers, VertexBuffer);
}

void
SetVertexArrayIndexBuffer(vertex_array *VertexArray, index_buffer **IndexBuffer)
{
    BindVertexArray(VertexArray);
    BindIndexBuffer(*IndexBuffer);
    VertexArray->IndexBuffer = *IndexBuffer;
    *IndexBuffer = NULL;
}

void
BindVertexArray(const vertex_array *VertexArray)
{
    glBindVertexArray(VertexArray->RendererId);
}

void
UnbindVertexArray(const vertex_array *VertexArray)
{
    glBindVertexArray(0);
}

const vertex_buffer_array *
GetVertexArrayVertexBuffers(const vertex_array *VertexArray)
{
    return VertexArray->VertexBuffers;
}

const
index_buffer *GetVertexArrayIndexBuffer(const vertex_array *VertexArray)
{
    return VertexArray->IndexBuffer;
}

#endif
