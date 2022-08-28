#include <glad/glad.h>
#ifdef PK_GLFW
#include <glfw/glfw3.h>
#endif

#include "Core/Core.hpp"
#include "OpenGLContext.hpp"
#include "Platform/Window.hpp"

#include <fstream>

namespace Piksela
{

std::string ReadFile(const std::string &path)
{
    std::string result;
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    } else
    {
        PK_ASSERT(false, "Could not open file '" + path + "'");
    }

    return result;
}

OpenGLContext::OpenGLContext(std::shared_ptr<Window> window) :
        mWindow(window)
{
#ifdef PK_GLFW
    glfwMakeContextCurrent(static_cast<GLFWwindow *>(window->GetNativeWindow()));
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    PK_ASSERT(status, "gladLoadGLLoader failed to initialize GLAD.");
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

    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };
    uint32_t indices[] = {
        0, 1, 2, 2, 3, 0
    };
    uint32_t vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    std::string vertBinary = ReadFile("C:/dev/Piksela/Assets/Shaders/StandardVert.spv");
    std::string fragBinary = ReadFile("C:/dev/Piksela/Assets/Shaders/StandardFrag.spv");

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertBinary.c_str(), vertBinary.size());
    glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);
    int compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    PK_ASSERT(compiled, "Failed to compile vertex shader.");

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragBinary.c_str(), fragBinary.size());
    glSpecializeShader(fragmentShader, "main", 0, nullptr, nullptr);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    PK_ASSERT(compiled, "Failed to compile fragment shader.");

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &compiled);
    PK_ASSERT(compiled, "Failed to link fragment shader and vertex shader.");
    glUseProgram(mShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
}

void OpenGLContext::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void OpenGLContext::ClearBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(mShaderProgram);
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenGLContext::ResizeViewport(uint32_t width, uint32_t height)
{
    CoreLogger::Trace("Viewport resized to [{0}, {1}].", width, height);
    glViewport(0, 0, width, height);
}

void OpenGLContext::SwapBuffers()
{
#ifdef PK_GLFW
    glfwSwapBuffers(static_cast<GLFWwindow *>(mWindow->GetNativeWindow()));
#endif
}

} // namespace Piksela