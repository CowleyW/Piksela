#include <glad/glad.h>

#include "Buffer.hpp"
#include "Context.hpp"
#include "Core/Core.hpp"
#include "Renderer.hpp"

namespace Piksela
{

std::shared_ptr<Context> Renderer::sContext;
Renderer::RendererData Renderer::sData;

void Renderer::Init(const std::shared_ptr<Window> &window)
{
    sContext = Context::Create(window);
    // sData.VertexBuffer = VertexBuffer::Create(sData.MaxPolys * 3 * sizeof(Vertex));
    // sData.VertexBuffer->Bind();
}

void Renderer::Shutdown()
{
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    sContext->SetClearColor(r, g, b, a);
}

void Renderer::ClearBuffer()
{
    sContext->ClearBuffer();
}

void Renderer::ResizeViewport(uint32_t width, uint32_t height)
{
    sContext->ResizeViewport(width, height);
}

void Renderer::SwapBuffers()
{
    sContext->SwapBuffers();
}

void Renderer::BeginScene()
{
}

void Renderer::EndScene()
{
}

} // namespace Piksela