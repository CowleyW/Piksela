#include "Renderer.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "Core/Core.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
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

    sData.VertexArray = VertexArray::Create();
    sData.VertexBuffer = VertexBuffer::Create(8 * sizeof(Vertex));
    sData.IndexBuffer = IndexBuffer::Create(36 * sizeof(uint32_t));
    sData.VertexArray->SetVertexBuffer(sData.VertexBuffer);
    sData.VertexArray->SetIndexBuffer(sData.IndexBuffer);

    sData.Texture = Texture::Create("C:/dev/Piksela/Assets/Textures/Stone_Pavement_DIFF.png");

    sData.StandardShader = Shader::Create("C:/dev/Piksela/Assets/Shaders/StandardShader.vert",
            "C:/dev/Piksela/Assets/Shaders/StandardShader.frag");
    sData.StandardShader->Bind();
    sData.StandardShader->SetInt("uTexture", 0);
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

void Renderer::BeginScene(const PerspectiveCamera &camera)
{
    sData.StandardShader->Bind();
    sData.StandardShader->SetViewProjectionMatrix(camera.GetViewProjectionMatrix());
}

void Renderer::EndScene()
{
    sData.Texture->Bind(0);
    sContext->DrawIndexed(sData.VertexArray);
}

} // namespace Piksela