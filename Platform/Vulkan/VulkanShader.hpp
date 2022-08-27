#pragma once

#include <volk.h>

#include "Engines/Rendering/Shader.hpp"

namespace Piksela
{

class VulkanShader : public Shader
{
public:
    VulkanShader(const std::string &vertexPath, const std::string &fragmentPath);

private:
    static std::string ReadSpirVFile(const std::string &path);
    // static VkShaderModule CreateShaderModule(const std::string &source);

private:
    VkPipelineShaderStageCreateInfo mVertexCreateInfo;
    VkPipelineShaderStageCreateInfo mFragmentCreateInfo;
};

} // namespace Piksela