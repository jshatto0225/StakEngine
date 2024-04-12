#include "OpenGLPlatform.h"

#ifdef SK_OPENGL

#include <string.h>

#include "Shader.h"
#include "Log.h"

struct shader
{
    u32 RendererId;
};

///////////////////////
// Private Interface //
///////////////////////

void
CompileShader(shader *Shader, const char *VertexShader, const char *FragmentShader)
{
    Shader->RendererId = glCreateProgram();
    u32 VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char *Src = VertexShader;
    glShaderSource(VertexShaderId, 1, &Src, NULL);
    glCompileShader(VertexShaderId);
    glAttachShader(Shader->RendererId, VertexShaderId);
    i32 Result;
    glGetShaderiv(VertexShaderId, GL_COMPILE_STATUS, &Result);
    if (Result == GL_FALSE)
    {
        i32 Size;
        glGetShaderiv(VertexShaderId, GL_INFO_LOG_LENGTH, &Size);
        char *Message = (char *)malloc(Size);
        glGetShaderInfoLog(VertexShaderId, Size, &Size, Message);
        LogCoreError("Failed to compile vertex shader: %s", Message);
        free(Message);
        return;
    }
    
    u32 FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    Src = FragmentShader;
    glShaderSource(FragmentShaderId, 1, &Src, NULL);
    glCompileShader(FragmentShaderId);
    glAttachShader(Shader->RendererId, FragmentShaderId);
    
    glGetShaderiv(FragmentShaderId, GL_COMPILE_STATUS, &Result);
    if (Result == GL_FALSE)
    {
        i32 Size;
        glGetShaderiv(FragmentShaderId, GL_INFO_LOG_LENGTH, &Size);
        char *Message = (char *)malloc(Size);
        glGetShaderInfoLog(FragmentShaderId, Size, &Size, Message);
        LogCoreError("Failed to compile fragment shader: %s", Message);
        free(Message);
        return;
    }
    
    glLinkProgram(Shader->RendererId);
    glValidateProgram(Shader->RendererId);
    
    glGetProgramiv(Shader->RendererId, GL_LINK_STATUS, &Result);
    if (Result == GL_FALSE)
    {
        i32 Size;
        glGetProgramiv(Shader->RendererId, GL_INFO_LOG_LENGTH, &Size);
        char *Message = (char *)malloc(Size);
        glGetProgramInfoLog(Shader->RendererId, Size, &Size, Message);
        LogCoreError("Failed to link shader: %s", Message);
        free(Message);
    }
    
    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);
}

shader_source
ParseShaderSource(const char *Path)
{
    shader_source Src = {};
    FILE *File;
    File = fopen(Path, "r");
    if (File == NULL)
    {
        LogCoreError("Failed to open shader file");
        return Src;
    }
    
    char Line[255] = {};
    char *VertexShader = 0;
    char *FragmentShader = 0;
    i32 VertexShaderLength = 0;
    i32 FragmentShaderLength = 0;
    shader_type CurrentType = SHADER_TYPE_NONE;
    
    while (fgets(Line, sizeof(Line), File))
    {
        switch (CurrentType)
        {
        case SHADER_TYPE_NONE:
            if (strncmp(Line, "#shader vertex", 14) == 0)
            {
                CurrentType = SHADER_TYPE_VERTEX;
                break;
            }
            if (strncmp(Line, "#shader fragment", 16) == 0)
            {
                CurrentType = SHADER_TYPE_FRAGMENT;
                break;
            }
            break;
            
        case SHADER_TYPE_VERTEX:
        {
            if (strncmp(Line, "#shader fragment", 16) == 0)
            {
                CurrentType = SHADER_TYPE_FRAGMENT;
                break;
            }
            i32 Len = (i32)strlen(Line);
            char *Result = (char *)realloc(VertexShader, VertexShaderLength + Len + 1);
            if (!Result)
            {
                LogCoreError("Failed to realloc vertex shader source");
                if (VertexShader)
                {
                    free(VertexShader);
                }
                free(VertexShader);
                return {};
            }
            VertexShader = Result;
            strcpy(VertexShader + VertexShaderLength, Line);
            VertexShaderLength += Len;
            break;
        }
        
        case SHADER_TYPE_FRAGMENT:
        {
            if (strncmp(Line, "#shader vertex", 14) == 0)
            {
                CurrentType = SHADER_TYPE_VERTEX;
                break;
            }
            i32 Len = (i32)strlen(Line);
            char *Result = (char *)realloc(FragmentShader, FragmentShaderLength + Len + 1);
            if (!Result)
            {
                LogCoreError("Failed to realloc fragment shader source");
                if (FragmentShader)
                {
                    free(FragmentShader);
                }
                return {};
            }
            FragmentShader = Result;
            strcpy(FragmentShader + FragmentShaderLength, Line);
            FragmentShaderLength += Len;
            break;
        }
        }
    }
    
    fclose(File);
    
    Src.VertexShader = VertexShader;
    Src.FragmentShader = FragmentShader;
    
    return Src;
}

//////////////////////
// Public Interface //
//////////////////////

shader *
CreateShader(const char *VertexShader, const char *FragmentShader)
{
    shader *Shader = (shader *)malloc(sizeof(shader));

    if (!Shader)
    {
        LogCoreError("Failed to allocate memory for shader");
        return NULL;
    }

    CompileShader(Shader, VertexShader, FragmentShader);

    return Shader;
}

shader *
CreateShader(const char *FilePath)
{
    shader *Shader = (shader *)malloc(sizeof(shader));
    
    if (!Shader) {
        LogCoreError("Faile to allocate memory for shader");
        return NULL;
    }
    
    shader_source Src = ParseShaderSource(FilePath);
    CompileShader(Shader, Src.VertexShader, Src.FragmentShader);
    
    if (Src.VertexShader)
    {
        free(Src.VertexShader);
    }
    if (Src.FragmentShader)
    {
        free(Src.FragmentShader);
    }
    
    return Shader;
}

void DestroyShader(shader **Shader)
{
    if (*Shader != NULL)
    {
        glDeleteProgram((*Shader)->RendererId);
        
        free(*Shader);
        
        *Shader = NULL;
    }
}

void
BindShader(const shader *Shader)
{
    glUseProgram(Shader->RendererId);
}

void
UnbindShader(const shader *Shader)
{
    glUseProgram(0);
}

#endif
