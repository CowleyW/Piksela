#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Core.hpp"
#include "Core/IO/FileReader.hpp"
#include "Engines/Rendering/Renderer.hpp"
#include "OpenGLShader.hpp"

namespace Piksela
{

OpenGLShader::OpenGLShader(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::string vertBinary = FileReader::Read("C:/dev/Piksela/Assets/Shaders/StandardShader.vert");
    std::string fragBinary = FileReader::Read("C:/dev/Piksela/Assets/Shaders/StandardShader.frag");

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertexSource = vertBinary.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    int compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    PK_ASSERT(compiled, "Failed to compile vertex shader.");

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragSource = fragBinary.c_str();
    glShaderSource(fragmentShader, 1, &fragSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    PK_ASSERT(compiled, "Failed to compile fragment shader.");

    mProgramID = glCreateProgram();
    glAttachShader(mProgramID, vertexShader);
    glAttachShader(mProgramID, fragmentShader);
    glLinkProgram(mProgramID);
    glGetProgramiv(mProgramID, GL_LINK_STATUS, &compiled);
    PK_ASSERT(compiled, "Failed to link fragment shader and vertex shader.");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

OpenGLShader::OpenGLShader(const std::string &name)
{
}

void OpenGLShader::Bind()
{
    glUseProgram(mProgramID);
}

void OpenGLShader::SetViewProjectionMatrix(const glm::mat4 &matrix)
{
    static int matricesLocation = glGetUniformLocation(mProgramID, "uViewProjection");

    glUniformMatrix4fv(matricesLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

} // namespace Piksela