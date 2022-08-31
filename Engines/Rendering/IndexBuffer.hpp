#pragma once

#include "Core/Core.hpp"

namespace Piksela
{

class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind() = 0;
    virtual uint32_t GetCount() = 0;

    static std::shared_ptr<IndexBuffer> Create(size_t size);
};

} // namespace Piksela