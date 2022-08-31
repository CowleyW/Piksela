#include "Renderer.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "Core/Core.hpp"
#include "VertexBuffer.hpp"

namespace Piksela
{

std::shared_ptr<Context> Renderer::sContext;
Renderer::RendererData Renderer::sData;

void Renderer::Init(const std::shared_ptr<Window> &window)
{
    sContext = Context::Create(window);

    sData.FramebufferWidth = window->GetWidth();
    sData.FramebufferHeight = window->GetHeight();
    sData.VertexBuffer = VertexBuffer::Create(sData.MaxPolys * 3 * sizeof(Vertex));
    sData.VertexBuffer->Bind();
    sData.IndexBuffer = IndexBuffer::Create(sData.MaxPolys * 2 * sizeof(uint32_t));
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
    sData.FramebufferWidth = width;
    sData.FramebufferHeight = height;
    sContext->ResizeViewport(width, height);
}

float Renderer::GetFramebufferAspectRatio()
{
    return sData.FramebufferWidth / (float)sData.FramebufferHeight;
}

void Renderer::SwapBuffers()
{
    sContext->SwapBuffers();
}

void Renderer::BeginScene(const std::shared_ptr<PerspectiveCamera> &camera)
{
    sData.IndexBuffer->Bind();
}

void Renderer::EndScene()
{
}

} // namespace Piksela