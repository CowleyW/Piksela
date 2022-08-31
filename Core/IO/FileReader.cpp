#include "FileReader.hpp"
#include "Core/Core.hpp"

#include <fstream>

namespace Piksela
{

std::string FileReader::Read(const std::string &filePath)
{
    std::string result;
    std::ifstream in(filePath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    } else
    {
        PK_ASSERT(false, "Could not open file '" + filePath + "'");
    }

    return result;
}

} // namespace Piksela