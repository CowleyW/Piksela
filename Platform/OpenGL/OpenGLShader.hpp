#pragma once

#include <glm/glm.hpp>

#include "Core/Core.hpp"
#include "Engines/Rendering/Shader.hpp"

namespace Piksela
{

class OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string &vertexPath, const std::string &fragmentPath);
    OpenGLShader(const std::string &name);

    virtual void Bind() override;
    virtual void SetViewProjectionMatrix(const glm::mat4 &matrix);

private:
    uint32_t mProgramID;
};

} // namespace Piksela