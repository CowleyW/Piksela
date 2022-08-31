#pragma once

#include "Core/Core.hpp"
#include "Platform/Window.hpp"
#include "VertexArray.hpp"

namespace Piksela
{

class Context
{
public:
    static std::shared_ptr<Context> Create(const std::shared_ptr<Window> &window);
    virtual ~Context() = default;

    virtual void SetClearColor(float r, float g, float b, float a) = 0;
    virtual void ClearBuffer() = 0;
    virtual void DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray) = 0;
    virtual void ResizeViewport(uint32_t width, uint32_t height) = 0;
    virtual void SwapBuffers() = 0;
};

} // namespace Piksela