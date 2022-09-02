#pragma once
#include "Core/Core.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

class DebugLayer
{
public:
    DebugLayer(const std::shared_ptr<Window> &window);
    ~DebugLayer();

    void BeginDebugRender();
    void EndDebugRender();
    void Render(float timestep);

private:
    std::shared_ptr<Window> mWindow;
};

} // namespace Piksela