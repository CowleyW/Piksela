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

    static void SetClearColor(float r, float g, float b, float a);
    static void ClearBuffer();
    static void ResizeViewport(uint32_t width, uint32_t height);
    static void SwapBuffers();

    static void BeginScene();
    static void EndScene();

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