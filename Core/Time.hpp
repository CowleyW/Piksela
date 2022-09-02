#pragma once

#include <chrono>

namespace Piksela
{

class Time
{
public:
    static void Init(float fixedTimestep);

    static void SetFixedTimestep(float fixedTimestep);
    static float GetFixedTimestep();
    static float GetTimestep();

private:
    static float sFixedTimestep;
    static std::chrono::time_point<std::chrono::high_resolution_clock> sCurrentTime;
};

} // namespace Piksela
