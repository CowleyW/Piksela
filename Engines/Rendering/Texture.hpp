#pragma once

#include "Core/Core.hpp"

namespace Piksela
{

class Texture
{
public:
    virtual ~Texture() = default;

    virtual void Bind(uint32_t slot) = 0;

    static std::shared_ptr<Texture> Create(const std::string &filePath);
};

} // namespace Piksela