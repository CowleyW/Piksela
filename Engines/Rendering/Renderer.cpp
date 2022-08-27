#include "Renderer.hpp"
#include "Buffer.hpp"
#include "Context.hpp"
#include "Core/Core.hpp"

namespace Piksela
{

std::shared_ptr<Context> Renderer::sContext;
Renderer::RendererData Renderer::sData;

void Renderer::Init(const std::shared_ptr<Window> &window)
{
    sContext = Context::Create(window);
    sData.VertexBuffer = VertexBuffer::Create(sData.MaxPolys * 3 * sizeof(Vertex));
    sData.VertexBuffer->Bind();
}

void Renderer::Shutdown()
{
    sContext->WaitIdle();
}

void Renderer::DrawFrame()
{
    sContext->DrawFrame();
}

void Renderer::ResizeViewport()
{
    sContext->ResizeViewport();
}

} // namespace Piksela