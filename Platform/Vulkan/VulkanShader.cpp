#include <fstream>

#include "Core/Core.hpp"
#include "VulkanContext.hpp"
#include "VulkanShader.hpp"

namespace Piksela
{

VulkanShader::VulkanShader(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::string vertexSource = ReadSpirVFile(vertexPath);
    std::string fragmentSource = ReadSpirVFile(fragmentPath);
}

std::string VulkanShader::ReadSpirVFile(const std::string &path)
{
    std::string source;
    std::ifstream in(path, std::ios::ate | std::ios::binary);
    if (!in.is_open())
    {
        // TODO: instead send error message and somehow
        // handle loading default shader
        PK_ASSERT(false, "Failed to open Spir-V file '{0}'.", path);
    }
    source.resize(in.tellg());
    in.seekg(0);
    in.read(&source[0], source.size());
    in.close();

    CoreLogger::Trace("Loaded file '{0}' of size {1} bytes.", path, source.size());
    return source;
}

} // namespace Piksela