#include "OpenGL.h"

#include "Platform.h"
#include "Renderer.h"
#include "Window.h"
#include "Log.h"
#include "Image.h"

struct vertex_buffer {
  u32 RendererId;
  buffer_layout *Layout;
};

struct index_buffer {
  u32 RendererId;
  u32 Count;
};

struct vertex_buffer_array {
  u64 Size;
  u64 Capacity;
  const vertex_buffer **Buffers;
};

struct vertex_array {
  vertex_buffer_array *VertexBuffers;
  index_buffer *IndexBuffer;
  u32 RendererId;
  u32 VertexBufferIndex;
};

struct shader {
  u32 RendererId;
};

struct texture {
  u32 RendererId;
  texture_specification Spec;
};

struct texture2d {
  u32 RendererId;
  texture_specification Spec;
  bool Loaded;
  const char *Path;
};

struct uniform_buffer {
  u32 RendererId;
};

GLGETSTRINGPROC glGetString;
GLBINDTEXTUREPROC glBindTexture;
GLBLENDFUNCPROC glBlendFunc;
GLCLEARPROC glClear;
GLCLEARCOLORPROC glClearColor;
GLDELETETEXTURESPROC glDeleteTextures;
GLDRAWELEMENTSPROC glDrawElements;
GLENABLEPROC glEnable;
GLGENTRXTURESPROC glGenTextures;
GLTEXPARAMETERIPROC glTexParameteri;
GLTEXSUBIMAGE2DPROC glTexSubImage2D;
GLVIEWPORTPROC glViewport;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLTEXSTORAGE2DPROC glTexStorage2D;
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
PFNGLCREATEBUFFERSPROC glCreateBuffers;
PFNGLNAMEDBUFFERDATAPROC glNamedBufferData;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;

void RenderApiBind() {
  // Core Functions
  glGetString = (GLGETSTRINGPROC)GetRendererApiProc("glGetString");
  glBindTexture = (GLBINDTEXTUREPROC)GetRendererApiProc("glBindTexture");
  glBlendFunc = (GLBLENDFUNCPROC)GetRendererApiProc("glBlendFunc");
  glClear = (GLCLEARPROC)GetRendererApiProc("glClear");
  glClearColor = (GLCLEARCOLORPROC)GetRendererApiProc("glClearColor");
  glDeleteTextures = (GLDELETETEXTURESPROC)GetRendererApiProc("glDeleteTextures");
  glDrawElements = (GLDRAWELEMENTSPROC)GetRendererApiProc("glDrawElements");
  glEnable = (GLENABLEPROC)GetRendererApiProc("glEnable");
  glGenTextures = (GLGENTRXTURESPROC)GetRendererApiProc("glGenTextures");
  glTexParameteri = (GLTEXPARAMETERIPROC)GetRendererApiProc("glTexParameteri");
  glTexSubImage2D = (GLTEXSUBIMAGE2DPROC)GetRendererApiProc("glTexSubImage2D");
  glViewport = (GLVIEWPORTPROC)GetRendererApiProc("glViewport");

  const unsigned char *Version = glGetString(GL_VERSION);
  LogCoreTrace("OpenGL Version %s", Version);

  // Other Functions
  glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)PlatformGetProcAddress("glBindTextureUnit");
  glGenBuffers = (PFNGLGENBUFFERSPROC)PlatformGetProcAddress("glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERPROC)PlatformGetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)PlatformGetProcAddress("glBufferData");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)PlatformGetProcAddress("glDeleteBuffers");
  glBufferSubData = (PFNGLBUFFERSUBDATAPROC)PlatformGetProcAddress("glBufferSubData");
  glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)PlatformGetProcAddress("glDebugMessageCallback");
  glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)PlatformGetProcAddress("glDebugMessageControl");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)PlatformGetProcAddress("glGenVertexArrays");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)PlatformGetProcAddress("glDeleteVertexArrays");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)PlatformGetProcAddress("glEnableVertexAttribArray");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)PlatformGetProcAddress("glVertexAttribPointer");
  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)PlatformGetProcAddress("glVertexAttribDivisor");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)PlatformGetProcAddress("glBindVertexArray");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)PlatformGetProcAddress("glCreateProgram");
  glCreateShader = (PFNGLCREATESHADERPROC)PlatformGetProcAddress("glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)PlatformGetProcAddress("glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC)PlatformGetProcAddress("glCompileShader");
  glAttachShader = (PFNGLATTACHSHADERPROC)PlatformGetProcAddress("glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)PlatformGetProcAddress("glLinkProgram");
  glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)PlatformGetProcAddress("glValidateProgram");
  glDeleteShader = (PFNGLDELETESHADERPROC)PlatformGetProcAddress("glDeleteShader");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC)PlatformGetProcAddress("glDeleteProgram");
  glUseProgram = (PFNGLUSEPROGRAMPROC)PlatformGetProcAddress("glUseProgram");
  glGetShaderiv = (PFNGLGETSHADERIVPROC)PlatformGetProcAddress("glGetShaderiv");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)PlatformGetProcAddress("glGetProgramiv");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)PlatformGetProcAddress("glGetShaderInfoLog");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)PlatformGetProcAddress("glGetProgramInfoLog");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)PlatformGetProcAddress("glGetUniformLocation");
  glUniform1i = (PFNGLUNIFORM1IPROC)PlatformGetProcAddress("glUniform1i");
  glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)PlatformGetProcAddress("glTexStorage2D");
  glCreateBuffers = (PFNGLCREATEBUFFERSPROC)PlatformGetProcAddress("glCreateBuffers");
  glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)PlatformGetProcAddress("glNamedBufferData");
  glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)PlatformGetProcAddress("glBindBufferBase");
  glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)PlatformGetProcAddress("glNamedBufferSubData");
}

static GLenum ShaderDataTypeToOpenGLType(shader_data_type Type) {
  switch (Type) {
  case SDT_FLOAT:
    return GL_FLOAT;
  case SDT_FLOAT2:
    return GL_FLOAT;
  case SDT_FLOAT3:
    return GL_FLOAT;
  case SDT_FLOAT4:
    return GL_FLOAT;
  case SDT_MAT3:
    return GL_FLOAT;
  case SDT_MAT4:
    return GL_FLOAT;
  case SDT_INT:
    return GL_INT;
  case SDT_INT2:
    return GL_INT;
  case SDT_INT3:
    return GL_INT;
  case SDT_INT4:
    return GL_INT;
  case SDT_BOOL:
    return GL_BOOL;
  default:
    LogCoreCritical("Unknown Data Type");
    return 0;
  }
}

static GLenum ImageFormatToOpenGLDataFormat(image_format Format) {
  switch (Format) {
  case IMAGE_FORMAT_R8:
    return GL_RED;
  case IMAGE_FORMAT_RGB8:
    return GL_RGB;
  case IMAGE_FORMAT_RGBA8:
    return GL_RGBA;
  case IMAGE_FORMAT_RGBA32F:
    return GL_RGBA;
  default:
    LogCoreError("Invalid Image Format");
    return GL_RGBA;
  }
}

static GLenum ImageFormatToOpenGLInternalFormat(image_format Format) {
  switch (Format) {
  case IMAGE_FORMAT_R8:
    return GL_R8;
  case IMAGE_FORMAT_RGB8:
    return GL_RGB8;
  case IMAGE_FORMAT_RGBA8:
    return GL_RGBA8;
  case IMAGE_FORMAT_RGBA32F:
    return GL_RGBA32F;
  default:
    LogCoreError("Invalid Image Format");
    return GL_RGBA8;
  }
}

static void APIENTRY OpenGLMessageCallback(u32 Source,
                                           u32 Type,
                                           u32 Id,
                                           u32 Severity,
                                           i32 Length,
                                           const char *Message,
                                           const void *UserParam) {
  switch (Severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    LogCoreCritical(Message);
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    LogCoreError(Message);
    break;
  case GL_DEBUG_SEVERITY_LOW:
    LogCoreWarn(Message);
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    LogCoreTrace(Message);
    break;
  }
}

bool RenderApiInitialized = false;
window *RenderApiWindow = NULL;

void RenderApiInit(window *Window) {
  if (RenderApiInitialized) {
    return;
  }

#pragma warning(disable : 4067)
#if defined(SK_DEBUG) or defined(SK_RELEASE)
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(OpenGLMessageCallback, NULL);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                        GL_DEBUG_SEVERITY_NOTIFICATION,
                        0,
                        NULL,
                        GL_FALSE);
#endif

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);

  RenderApiWindow = Window;

  RenderApiInitialized = true;
}

void RenderApiShutdown() {
  RenderApiInitialized = false;
}

void RenderApiSwapBuffers() {
  SwapWindowBuffers(RenderApiWindow);
}

void RenderApiSetClearColor(f32 Red, f32 Green, f32 Blue, f32 Alpha) {
  glClearColor(Red, Green, Blue, Alpha);
}

void RenderApiClear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderApiSetViewport(i32 X, i32 Y, i32 Width, i32 Height) {
  glViewport(X, Y, Width, Height);
}

void RenderApiSetLineWidth(f32 Width) {
  // TODO:
}

void RenderApiDrawIndexed(const vertex_array *VertexArray, u32 Count) {
  if (Count == 0) {
    Count = GetIndexBufferIndexCount(GetVertexArrayIndexBuffer(VertexArray));
  }
  glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, NULL);
}

void RenderApiDrawLines(const vertex_array *VertexArray, u32 Count) {
  //TODO:
}

vertex_buffer *CreateVertexBuffer(u32 Size) {
  vertex_buffer *VertexBuffer = (vertex_buffer *)malloc(sizeof(vertex_buffer));

  if (!VertexBuffer) {
    LogCoreError("Failed to allocate memory for vertex buffer");
    return NULL;
  }
  
  glGenBuffers(1, &VertexBuffer->RendererId);
  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
  glBufferData(GL_ARRAY_BUFFER, Size, NULL, GL_DYNAMIC_DRAW);

  return VertexBuffer;
}

vertex_buffer *CreateVertexBuffer(const f32 *Vertices, u32 Size) {
  vertex_buffer *VertexBuffer = (vertex_buffer *)malloc(sizeof(vertex_buffer));

  if (!VertexBuffer) {
    LogCoreError("Failed to allocate memory for vertex buffer");
    return NULL;
  }
  
  glGenBuffers(1, &VertexBuffer->RendererId);
  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
  glBufferData(GL_ARRAY_BUFFER, Size, Vertices, GL_DYNAMIC_DRAW);

  return VertexBuffer;
}

void DestroyVertexBuffer(vertex_buffer **VertexBuffer) {
  if (*VertexBuffer) {
    glDeleteBuffers(1, &(*VertexBuffer)->RendererId);

    DestroyBufferLayout(&(*VertexBuffer)->Layout);
  
    free(*VertexBuffer);

    *VertexBuffer = NULL;
  }
}

void BindVertexBuffer(const vertex_buffer *VertexBuffer) {
  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
}

void UnbindVertexBuffer(const vertex_buffer *Vertexbuffer) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SetVertexBufferLayout(vertex_buffer *VertexBuffer, buffer_layout **BufferLayout) {
  VertexBuffer->Layout = *BufferLayout;
  *BufferLayout = NULL;
}

void SetVertexBufferData(vertex_buffer *VertexBuffer, const void *Data, u32 Size) {
  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer->RendererId);
  glBufferSubData(GL_ARRAY_BUFFER, 0, Size, Data);
}

const buffer_layout *GetVertexBufferLayout(const vertex_buffer *VertexBuffer) {
  return VertexBuffer->Layout;
}

index_buffer *CreateIndexBuffer() {
  index_buffer *IndexBuffer = (index_buffer *)malloc(sizeof(index_buffer));

  if (!IndexBuffer) {
    LogCoreError("Failed to allocate memory for index buffer");
    return NULL;
  }
  
  IndexBuffer->Count = 0;

  glGenBuffers(1, &IndexBuffer->RendererId);

  return IndexBuffer;
}

index_buffer *CreateIndexBuffer(u32 *Indices, u32 Count) {
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

void DestroyIndexBuffer(index_buffer **IndexBuffer) {
  if (*IndexBuffer) {
    glDeleteBuffers(1, &(*IndexBuffer)->RendererId);

    free(*IndexBuffer);

    *IndexBuffer = NULL;
  }
}

void BindIndexBuffer(const index_buffer *IndexBuffer) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer->RendererId);
}

void UnbindIndexBuffer(const index_buffer *IndexBuffer) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 GetIndexBufferIndexCount(const index_buffer *IndexBuffer) {
  return IndexBuffer->Count;
}

vertex_buffer_array *CreateVertexBufferArray(u64 Capacity) {
  vertex_buffer_array *Array = (vertex_buffer_array *)malloc(sizeof(vertex_buffer_array));

  if (!Array) {
    LogCoreError("Failed to allocate memory for vertex buffer array");
    return NULL;
  }

  Array->Capacity = Capacity;
  Array->Size = 0;
  Array->Buffers = (const vertex_buffer **)malloc(sizeof(vertex_buffer *) * Array->Capacity);

  return Array;
}

void AddVertexBufferToArray(vertex_buffer_array *Array, const vertex_buffer *VertexBuffer) {
  if (Array->Size == Array->Capacity) {
    Array->Capacity *= 2;
    Array->Buffers = (const vertex_buffer **)realloc((void *)Array->Buffers, sizeof(vertex_buffer *) * Array->Capacity);

    if (!Array->Buffers) {
      LogCoreError("Failed to reallocate vertex buffer array");
      return;
    }
  }

  Array->Buffers[Array->Size] = VertexBuffer;
  Array->Size++;
}

void DestroyVertexBufferArray(vertex_buffer_array **Array) {
  if (*Array) {
    free(*Array);

    *Array = NULL;
  }
}

vertex_array *CreateVertexArray() {
  vertex_array *VertexArray = (vertex_array *)malloc(sizeof(vertex_array));

  if (!VertexArray) {
    LogCoreError("Failed to allocate memory for vertex array");
    return NULL;
  }

  VertexArray->VertexBuffers = CreateVertexBufferArray(16);
  
  glGenVertexArrays(1, &VertexArray->RendererId);
  VertexArray->VertexBufferIndex = 0;

  return VertexArray;
}

void DestroyVertexArray(vertex_array **VertexArray) {
  if (*VertexArray) {
    glDeleteVertexArrays(1, &(*VertexArray)->RendererId);

    DestroyIndexBuffer(&(*VertexArray)->IndexBuffer);
    DestroyVertexBufferArray(&(*VertexArray)->VertexBuffers);

    free(*VertexArray);

    *VertexArray = NULL;
  }
}

void AddVertexBufferToVertexArray(vertex_array *VertexArray, const vertex_buffer *VertexBuffer) {
  BindVertexArray(VertexArray);
  BindVertexBuffer(VertexBuffer);
  const buffer_layout *Layout = GetVertexBufferLayout(VertexBuffer);
  for (u64 i = 0; i < Layout->Size; i++) {
    buffer_element Element = Layout->Elements[i];
    switch (Element.Type) {
    case SDT_INT:
    case SDT_INT2:
    case SDT_INT3:
    case SDT_INT4:
    case SDT_FLOAT:
    case SDT_FLOAT2:
    case SDT_FLOAT3:
    case SDT_FLOAT4: {
      glEnableVertexAttribArray(VertexArray->VertexBufferIndex);
      glVertexAttribPointer(VertexArray->VertexBufferIndex,
                            GetBufferElementComponentCount(&Element),
                            ShaderDataTypeToOpenGLType(Element.Type),
                            Element.Normalized ? GL_TRUE : GL_FALSE,
                            Layout->Stride,
                            (const void *)Element.Offset);
      VertexArray->VertexBufferIndex++;
      break;
    }
    case SDT_BOOL: {
      glEnableVertexAttribArray(VertexArray->VertexBufferIndex);
      glVertexAttribPointer(VertexArray->VertexBufferIndex,
                            GetBufferElementComponentCount(&Element),
                            ShaderDataTypeToOpenGLType(Element.Type),
                            Element.Normalized ? GL_TRUE : GL_FALSE,
                            Layout->Stride,
                            (const void *)Element.Offset);
      VertexArray->VertexBufferIndex++;
      break;
    }
    case SDT_MAT3:
    case SDT_MAT4: {
      u8 Count = GetBufferElementComponentCount(&Element);
      for (u8 i = 0; i < Count; i++) {
        glEnableVertexAttribArray(VertexArray->VertexBufferIndex);
        glVertexAttribPointer(VertexArray->VertexBufferIndex,
                              Count,
                              ShaderDataTypeSize(Element.Type),
                              Element.Normalized ? GL_TRUE : GL_FALSE,
                              Layout->Stride,
                              (const void *)(Element.Offset + sizeof(f32) * Count * i));
        glVertexAttribDivisor(VertexArray->VertexBufferIndex, 1);
        VertexArray->VertexBufferIndex++;
      }
    }
    }
  }
  AddVertexBufferToArray(VertexArray->VertexBuffers, VertexBuffer);
}

void SetVertexArrayIndexBuffer(vertex_array *VertexArray, index_buffer **IndexBuffer) {
  BindVertexArray(VertexArray);
  BindIndexBuffer(*IndexBuffer);
  VertexArray->IndexBuffer = *IndexBuffer;
  *IndexBuffer = NULL;
}

void BindVertexArray(const vertex_array *VertexArray) {
  glBindVertexArray(VertexArray->RendererId);
}

void UnbindVertexArray(const vertex_array *VertexArray) {
  glBindVertexArray(0);
}

const vertex_buffer_array *GetVertexArrayVertexBuffers(const vertex_array *VertexArray) {
  return VertexArray->VertexBuffers;
}

const index_buffer *GetVertexArrayIndexBuffer(const vertex_array *VertexArray) {
  return VertexArray->IndexBuffer;
}

shader *CreateShader(const char *VertexShader, const char *FragmentShader) {
  shader *Shader = (shader *)malloc(sizeof(shader));

  if (!Shader) {
    LogCoreError("Failed to allocate memory for shader");
    return NULL;
  }

  CompileShader(Shader, VertexShader, FragmentShader);

  return Shader;
}

shader *CreateShader(const char *FilePath) {
  shader *Shader = (shader *)malloc(sizeof(shader));

  if (!Shader) {
    LogCoreError("Faile to allocate memory for shader");
    return NULL;
  }
  
  shader_source Src = ParseShaderSource(FilePath);
  CompileShader(Shader, Src.VertexShader, Src.FragmentShader);

  if (Src.VertexShader) {
    free(Src.VertexShader);
  }
  if (Src.FragmentShader) {
    free(Src.FragmentShader);
  }

  return Shader;
}

void CompileShader(shader *Shader, const char *VertexShader, const char *FragmentShader) {
  Shader->RendererId = glCreateProgram();
  u32 VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  const char *Src = VertexShader;
  glShaderSource(VertexShaderId, 1, &Src, NULL);
  glCompileShader(VertexShaderId);
  glAttachShader(Shader->RendererId, VertexShaderId);
  i32 Result;
  glGetShaderiv(VertexShaderId, GL_COMPILE_STATUS, &Result);
  if (Result == GL_FALSE) {
    i32 Size;
    glGetShaderiv(VertexShaderId, GL_INFO_LOG_LENGTH, &Size);
    char *Message = (char *)malloc(Size);
    glGetShaderInfoLog(VertexShaderId, Size, &Size, Message);
    LogCoreError("Failed to compile shader: %s", Message);
    free(Message);
  }

  u32 FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  Src = FragmentShader;
  glShaderSource(FragmentShaderId, 1, &Src, NULL);
  glCompileShader(FragmentShaderId);
  glAttachShader(Shader->RendererId, FragmentShaderId);

  glGetShaderiv(FragmentShaderId, GL_COMPILE_STATUS, &Result);
  if (Result == GL_FALSE) {
    i32 Size;
    glGetShaderiv(FragmentShaderId, GL_INFO_LOG_LENGTH, &Size);
    char *Message = (char *)malloc(Size);
    glGetShaderInfoLog(FragmentShaderId, Size, &Size, Message);
    LogCoreError("Failed to compile shader: %s", Message);
    free(Message);
  }

  glLinkProgram(Shader->RendererId);
  glValidateProgram(Shader->RendererId);

  glGetProgramiv(Shader->RendererId, GL_LINK_STATUS, &Result);
  if (Result == GL_FALSE) {
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

void DestroyShader(shader **Shader) {
  if (*Shader) {
    glDeleteProgram((*Shader)->RendererId);
    
    free(*Shader);
    
    *Shader = NULL;
  }
}

shader_source ParseShaderSource(const char *Path) {
  shader_source Src = {};
  FILE *File = fopen(Path, "r");
  if (File == NULL) {
    LogCoreError("Failed to open shader file");
    return Src;
  }

  char Line[255] = {};
  char *VertexShader = 0;
  char *FragmentShader = 0;
  i32 VertexShaderLength = 0;
  i32 FragmentShaderLength = 0;
  shader_type CurrentType = SHADER_TYPE_NONE;

  while (fgets(Line, sizeof(Line), File)) {
    switch (CurrentType) {
    case SHADER_TYPE_NONE:
      if (strncmp(Line, "#shader vertex", 14) == 0) {
        CurrentType = SHADER_TYPE_VERTEX;
        break;
      }
      if (strncmp(Line, "#shader fragment", 16) == 0) {
        CurrentType = SHADER_TYPE_FRAGMENT;
        break;
      }
      break;

    case SHADER_TYPE_VERTEX: {
      if (strncmp(Line, "#shader fragment", 16) == 0) {
        CurrentType = SHADER_TYPE_FRAGMENT;
        break;
      }
      i32 Len = strlen(Line);
      VertexShader = (char *)realloc(VertexShader, VertexShaderLength + Len + 1);
      strcpy(VertexShader + VertexShaderLength, Line);
      VertexShaderLength += Len;
      break;
    }
      
    case SHADER_TYPE_FRAGMENT: {
      if (strncmp(Line, "#shader vertex", 14) == 0) {
        CurrentType = SHADER_TYPE_VERTEX;
        break;
      }
      i32 Len = strlen(Line);
      FragmentShader = (char *)realloc(FragmentShader, FragmentShaderLength + Len + 1);
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

void BindShader(const shader *Shader) {
  glUseProgram(Shader->RendererId);
}

void UnbindShader(const shader *Shader) {
  glUseProgram(0);
}

texture *CreateTexture(const texture_specification *TextureSpecification) {
  texture *Texture = (texture *)malloc(sizeof(texture));

  if (!Texture) {
    LogCoreError("Failed to allocate memory for texture");
    return NULL;
  }
  
  glGenTextures(1, &Texture->RendererId);

  return Texture;
}

void DestroyTexture(texture **Texture) {
  if (*Texture) {
    glDeleteTextures(1, &(*Texture)->RendererId);

    free(*Texture);

    *Texture = NULL;
  }
}

const texture_specification *GetTextureSpecification(const texture *Tex) {
  return &Tex->Spec;
}

u32 GetTextureWidth(const texture *Tex) {
  return Tex->Spec.Width;
}

u32 GetTextureHeight(const texture *Tex) {
  return Tex->Spec.Height;
}

void SetTextureData(texture *Tex, void *Data, u32 Size) {

}

void BindTexture(texture *Tex, u32 Slot) {

}

bool IsTextureLoaded(texture *Tex) {
  return false;
}

bool CompareTexture(const texture *Tex1, const texture *Tex2) {
  return Tex1->RendererId == Tex2->RendererId;
}

texture2d *CreateTexture2D(const char *Path) {
  texture2d *Tex = (texture2d *)malloc(sizeof(texture2d));

  if (!Tex) {
    LogCoreError("Failed to allocate memory for texture 2d");
    return NULL;
  }
  
  Tex->Loaded = true;
  Tex->Path = Path;

  // LOAD IMAGE
  image *Image;
  Image = CreateImage(Path);
  switch (Image->Data.Channels) {
  case 4:
    Tex->Spec.Format = IMAGE_FORMAT_RGBA8;
    break;
  case 3:
    Tex->Spec.Format = IMAGE_FORMAT_RGB8;
    break;
  case 1:
    Tex->Spec.Format = IMAGE_FORMAT_R8;
    break;
  default:
    break;
  }
  image_data ImageData = Image->Data;
  Tex->Spec.Width = ImageData.Width;
  Tex->Spec.Height = ImageData.Height;

  glGenTextures(1, &Tex->RendererId);
  glBindTexture(GL_TEXTURE_2D, Tex->RendererId);
  glTexStorage2D(GL_TEXTURE_2D,
                 1,
                 ImageFormatToOpenGLInternalFormat(Tex->Spec.Format),
                 Tex->Spec.Width,
                 Tex->Spec.Height);

  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /* GL_LINEAR */);

  glTexSubImage2D(GL_TEXTURE_2D,
                  0,
                  0,
                  0,
                  Tex->Spec.Width,
                  Tex->Spec.Height,
                  ImageFormatToOpenGLDataFormat(Tex->Spec.Format),
                  GL_UNSIGNED_BYTE,
                  Image->Bytes);

  return Tex;
}

texture2d *CreateTexture2D(const texture_specification *TextureSpecification) {
  texture2d *Tex = (texture2d *)malloc(sizeof(texture2d));

  if (!Tex) {
    LogCoreError("Failed to allocate memory for texture 2d");
    return NULL;
  }

  Tex->Spec = *TextureSpecification;
  glGenTextures(1, &Tex->RendererId);
  glBindTexture(GL_TEXTURE_2D, Tex->RendererId);
  glTexStorage2D(GL_TEXTURE_2D,
                 1,
                 ImageFormatToOpenGLInternalFormat(Tex->Spec.Format),
                 Tex->Spec.Width,
                 Tex->Spec.Height);

  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /* GL_LINEAR */);

  Tex->Loaded = false;

  return Tex;
}

void DestroyTexture2D(texture2d **Tex) {
  if (*Tex) {
    glDeleteTextures(1, &(*Tex)->RendererId);

    free(*Tex);

    *Tex = NULL;
  }
}

const texture_specification *GetTexture2DSpecification(texture2d *Tex) {
  return &Tex->Spec;
}

u32 GetTexture2DWidth(texture2d *Tex) {
  return Tex->Spec.Width;
}

u32 GetTexture2DHeight(texture2d *Tex) {
  return Tex->Spec.Height;
}

u32 GetRendererId(texture2d *Tex) {
  return Tex->RendererId;
}

const char *GetTexture2DFiletPath(texture2d *Tex) {
  return Tex->Path;
}

void SetTexture2DData(texture2d *Tex, void *Data, u32 Size) {
  glBindTexture(GL_TEXTURE_2D, Tex->RendererId);
  glTexSubImage2D(GL_TEXTURE_2D,
                  0,
                  0,
                  0,
                  Tex->Spec.Width,
                  Tex->Spec.Height,
                  ImageFormatToOpenGLDataFormat(Tex->Spec.Format),
                  GL_UNSIGNED_BYTE,
                  Data);
  Tex->Loaded = true;
}

void BindTexture2D(const texture2d *Tex, u32 Slot) {
  //glActiveTexture(GL_TEXTURE0 + slot);
  //glBindTexture(GL_TEXTURE_2D, this->renderer_id);
  glBindTextureUnit(Slot, Tex->RendererId);
}

bool IsTexture2DLoaded(const texture2d *Tex) {
  return Tex->Loaded;
}

bool CompareTexture2D(const texture2d *Tex1, const texture2d *Tex2) {
  return Tex1->RendererId == Tex2->RendererId;
}

uniform_buffer *CreateUniformBuffer(u32 Size, u32 Binding) {
  uniform_buffer *UniformBuffer = (uniform_buffer *)malloc(sizeof(uniform_buffer));

  if (!UniformBuffer) {
    LogCoreError("Failed to allocate memory for uniform buffer");
    return NULL;
  }
  
  glCreateBuffers(1, &UniformBuffer->RendererId);
  glNamedBufferData(UniformBuffer->RendererId, Size, NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, Binding, UniformBuffer->RendererId);

  return UniformBuffer;
}

void DestroyUniformBuffer(uniform_buffer **UniformBuffer) {
  if (*UniformBuffer) {
    glDeleteBuffers(1, &(*UniformBuffer)->RendererId);

    free(*UniformBuffer);

    *UniformBuffer = NULL;
  }
}

void SetUniformBufferData(uniform_buffer *UniformBuffer, const void *Data, u32 Size, u32 Offset) {
  glNamedBufferSubData(UniformBuffer->RendererId, Offset, Size, Data);
}
