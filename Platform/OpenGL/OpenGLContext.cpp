#include <glad/glad.h>
#ifdef PK_GLFW
#include <glfw/glfw3.h>
#endif

#include "Core/Core.hpp"
#include "OpenGLContext.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

void OpenGLContext::DebugCallback(
        uint32_t source,
        uint32_t type,
        uint32_t id,
        uint32_t severity,
        int length,
        const char *message,
        const void *userParam)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        {
            CoreLogger::Trace(message);
            break;
        }
        case GL_DEBUG_SEVERITY_LOW:
        {
            CoreLogger::Warn(message);
            break;
        }
        case GL_DEBUG_SEVERITY_MEDIUM:
        {
            CoreLogger::Warn(message);
            break;
        }
        case GL_DEBUG_SEVERITY_HIGH:
        {
            CoreLogger::Error(message);
            break;
        }
    }
}

OpenGLContext::OpenGLContext(std::shared_ptr<Window> window) :
        mWindow(window)
{
#ifdef PK_GLFW
    glfwMakeContextCurrent(static_cast<GLFWwindow *>(window->GetNativeWindow()));
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    PK_ASSERT(status, "gladLoadGLLoader failed to initialize GLAD.");
#endif

#ifdef PK_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLContext::DebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

    glViewport(0, 0, window->GetWidth(), window->GetHeight());

    CoreLogger::Trace("OpenGL Info:");
    CoreLogger::Trace("    Vendor: {0}", glGetString(GL_VENDOR));
    CoreLogger::Trace("    Renderer: {0}", glGetString(GL_RENDERER));
    CoreLogger::Trace("    Version: {0}", glGetString(GL_VERSION));

    int versionMajor;
    int versionMinor;
    glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
    PK_ASSERT((versionMajor > 4) || (versionMajor == 4 && versionMinor >= 6), "Minimum OpenGL version supported is version 4.6");
}

void OpenGLContext::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void OpenGLContext::ClearBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLContext::DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray)
{
    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void OpenGLContext::ResizeViewport(uint32_t width, uint32_t height)
{
    CoreLogger::Trace("Viewport size: [{0}, {1}].", width, height);
    glViewport(0, 0, width, height);
}

void OpenGLContext::SwapBuffers()
{
#ifdef PK_GLFW
    glfwSwapBuffers(static_cast<GLFWwindow *>(mWindow->GetNativeWindow()));
#endif
}

} // namespace Piksela