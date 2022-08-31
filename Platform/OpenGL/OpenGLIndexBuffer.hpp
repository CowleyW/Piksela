#pragma once

#include "Engines/Rendering/IndexBuffer.hpp"

namespace Piksela
{

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(size_t size);

    virtual void Bind() override;

private:
    uint32_t mIndexBufferID;
};

} // namespace Piksela