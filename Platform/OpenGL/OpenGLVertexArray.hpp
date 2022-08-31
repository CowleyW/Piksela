#pragma once

#include "Core/Core.hpp"
#include "Engines/Rendering/IndexBuffer.hpp"
#include "Engines/Rendering/VertexArray.hpp"
#include "Engines/Rendering/VertexBuffer.hpp"

namespace Piksela
{

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual void SetVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) override;
    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) override;

    virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() override;
    virtual std::shared_ptr<IndexBuffer> GetIndexBuffer() override;

private:
    uint32_t mVertexArrayID;

    std::shared_ptr<VertexBuffer> mVertexBuffer;
    std::shared_ptr<IndexBuffer> mIndexBuffer;
};

} // namespace Piksela