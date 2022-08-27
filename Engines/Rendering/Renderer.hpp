#pragma once

#include <glm/glm.hpp>

#include "Context.hpp"
#include "Core/Core.hpp"
#include "Engines/Rendering/Buffer.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

class Renderer
{
public:
    static void Init(const std::shared_ptr<Window> &window);
    static void Shutdown();

    static void DrawFrame();
    static void ResizeViewport();

    static std::shared_ptr<Context> GetContext()
    {
        return sContext;
    }

private:
    static std::shared_ptr<Context> sContext;

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
    };

    struct RendererData
    {
        // Max amount of polys per draw call
        static const uint32_t MaxPolys = 5000;

        std::shared_ptr<VertexBuffer> VertexBuffer;
    };
    static RendererData sData;
};

} // namespace Piksela