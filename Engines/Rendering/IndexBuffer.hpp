#pragma once

#include "Core/Core.hpp"

namespace Piksela
{

class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind() = 0;

    static std::shared_ptr<IndexBuffer> Create(size_t size);
};

} // namespace Piksela