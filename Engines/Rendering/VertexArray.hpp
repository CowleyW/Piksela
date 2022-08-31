#pragma once

#include "Core/Core.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"

namespace Piksela
{

class VertexArray
{
public:
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual void SetVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) = 0;
    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) = 0;

    virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() = 0;
    virtual std::shared_ptr<IndexBuffer> GetIndexBuffer() = 0;

    static std::shared_ptr<VertexArray> Create();
};

} // namespace Piksela