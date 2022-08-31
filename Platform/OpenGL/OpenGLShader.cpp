#include <glad/glad.h>

#include <stb/stb_image.h>
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

#if 0
    int width, height, numChannels;
    uint8_t *data = stbi_load("C:/dev/Piksela/Assets/Textures/Old_Brick_DIFF.png", &width, &height, &numChannels, 0);
    PK_ASSERT(data, "stbi_load failed to load texture.");
    uint32_t textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUseProgram(mProgramID);
    int location = glGetUniformLocation(mProgramID, "uTexture");
    glUniform1i(location, 0);
    glBindTextureUnit(0, textureID);
    free(data);
#endif
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

void OpenGLShader::SetInt(const std::string &name, int value)
{
    int location = glGetUniformLocation(mProgramID, name.c_str());
    glUniform1i(location, value);
}

} // namespace Piksela