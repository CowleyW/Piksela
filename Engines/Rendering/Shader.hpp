#pragma once

#include <glm/glm.hpp>

#include "Core/Core.hpp"

namespace Piksela
{

class Shader
{
public:
    virtual ~Shader() = default;

    virtual void Bind() = 0;
    virtual void SetViewProjectionMatrix(const glm::mat4 &matrix) = 0;

    static std::shared_ptr<Shader> Create(const std::string &vertexPath, const std::string &fragmentPath);
    static std::shared_ptr<Shader> Create(const std::string &name);
};

} // namespace Piksela