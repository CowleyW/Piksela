#include <chrono>

#include "Time.hpp"

namespace Piksela
{

float Time::sFixedTimestep;
std::chrono::time_point<std::chrono::high_resolution_clock> Time::sCurrentTime;

void Time::Init(float fixedTimestep)
{
    SetFixedTimestep(fixedTimestep);
    sCurrentTime = std::chrono::high_resolution_clock::now();
}

void Time::SetFixedTimestep(float fixedTimestep)
{
    sFixedTimestep = fixedTimestep;
}

float Time::GetFixedTimestep()
{
    return sFixedTimestep;
}

/**
 * @brief Get the current timestep. Should not be called from Update() functions.
 *
 * @return float
 */
float Time::GetTimestep()
{
    auto time = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration<float>(time - sCurrentTime).count();
    sCurrentTime = time;

    return frameTime;
}

} // namespace Piksela