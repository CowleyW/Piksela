#pragma once

#include "Engines/Rendering/IndexBuffer.hpp"

namespace Piksela
{

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(size_t size);
    ~OpenGLIndexBuffer();

    virtual void Bind() override;
    virtual uint32_t GetCount() override;

private:
    uint32_t mIndexBufferID;

    uint32_t mCount;
};

} // namespace Piksela