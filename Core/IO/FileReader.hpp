#pragma once

#include "Core/Core.hpp"

namespace Piksela
{

class FileReader
{
public:
    static std::string Read(const std::string &filePath);
};

} // namespace Piksela