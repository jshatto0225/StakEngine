#include "OpenGLRendererAPI.h"

#include <algorithm>

#include "OpenGLBindings.h"
#include "Types.h"
#include "Log.h"
#include "PlatformManager.h"
#include "Renderer2D.h"

void APIENTRY OpenGLMessageCallback(u32 source,
                                    u32 type,
                                    u32 id,
                                    u32 severity,
                                    i32 length,
                                    const char* message,
                                    const void* userParam)
{
    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
        {
            SK_CORE_CRITICAL(message);
        } return;
        case GL_DEBUG_SEVERITY_MEDIUM:
        {
            SK_CORE_ERROR(message);
        } return;
        case GL_DEBUG_SEVERITY_LOW:
        {
            SK_CORE_WARN(message);
        } return;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        {
            SK_CORE_TRACE(message);
        }
    }
}

OpenGLRendererAPI::OpenGLRendererAPI(const Unique<Window>& window)
{
    mContext = PlatformManager::NewContext(window);

#ifdef SK_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE,
                          GL_DONT_CARE,
                          GL_DEBUG_SEVERITY_NOTIFICATION,
                          0,
                          nullptr,
                          GL_FALSE);
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
}

void OpenGLRendererAPI::DrawIndexed(Shared<VertexArray> vao, u32 count)
{
    vao->Bind();
    u32 newCount = count ? count : vao->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, newCount, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::DrawLines(Shared<VertexArray> vao, u32 count)
{
    vao->Bind();
    glDrawArrays(GL_LINES, 0, count);
}

void OpenGLRendererAPI::SetLineWidth(f32 width)
{
    glLineWidth(width);
}

void OpenGLRendererAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::SetClearColor(const Vec4f& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::SetViewport(const Vec4u& viewport)
{
    glViewport(viewport(0), viewport(1), viewport(2), viewport(3));
}
