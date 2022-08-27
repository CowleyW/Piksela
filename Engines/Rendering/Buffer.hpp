#pragma once

#include "Core/Core.hpp"

namespace Piksela
{

class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;

    virtual void Bind() = 0;

    static std::shared_ptr<VertexBuffer> Create(size_t size);
};

} // namespace Piksela