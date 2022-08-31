#pragma once

#include "Core/Core.hpp"
#include "Engines/Rendering/Context.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

class OpenGLContext : public Context
{
public:
    OpenGLContext(const std::shared_ptr<Window> window);

    virtual void SetClearColor(float r, float g, float b, float a) override;
    virtual void ClearBuffer() override;
    virtual void DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray) override;
    virtual void ResizeViewport(uint32_t width, uint32_t height) override;
    virtual void SwapBuffers() override;

    static void DebugCallback(
            uint32_t source,
            uint32_t type,
            uint32_t id,
            uint32_t severity,
            int length,
            const char *message,
            const void *userParam);

private:
    std::shared_ptr<Window> mWindow;

    uint32_t mVertexArray;
    uint32_t mIndexBuffer;
};

} // namespace Piksela