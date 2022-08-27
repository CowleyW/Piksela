#pragma once

#include "Core/Core.hpp"

namespace Piksela
{

class Shader
{
public:
    virtual ~Shader() = default;

    static std::shared_ptr<Shader> Create(const std::string &vertexPath, const std::string &fragmentPath);
};

} // namespace Piksela