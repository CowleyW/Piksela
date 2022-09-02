#pragma once

#include <glm/glm.hpp>

#include "Camera.hpp"
#include "Context.hpp"
#include "Core/Core.hpp"
#include "IndexBuffer.hpp"
#include "Platform/Window.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"

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
    static float GetFramebufferAspectRatio();
    static void SwapBuffers();

    static void BeginScene(const PerspectiveCamera &camera);
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
        static const uint32_t MaxPolys = 4;

        uint32_t FramebufferWidth;
        uint32_t FramebufferHeight;

        std::shared_ptr<VertexBuffer> VertexBuffer;
        std::shared_ptr<IndexBuffer> IndexBuffer;
        std::shared_ptr<VertexArray> VertexArray;

        std::shared_ptr<Shader> StandardShader;

        std::shared_ptr<Texture> Texture;
    };
    static RendererData sData;
};

} // namespace Piksela