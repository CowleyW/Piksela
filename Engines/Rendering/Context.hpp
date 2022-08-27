#pragma once

#include "Buffer.hpp"
#include "Core/Core.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

class Context
{
public:
    static std::shared_ptr<Context> Create(const std::shared_ptr<Window> &window);
    virtual ~Context() = default;

    virtual void DrawFrame() = 0;
    virtual void ResizeViewport() = 0;
    virtual void WaitIdle() = 0;
};

} // namespace Piksela