#pragma once

#include "Core/Core.hpp"
#include "Engines/Rendering/VertexBuffer.hpp"

namespace Piksela
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(size_t size);
    ~OpenGLVertexBuffer();

    virtual void Bind() override;

private:
    uint32_t mVertexBufferID;
};

} // namespace Piksela