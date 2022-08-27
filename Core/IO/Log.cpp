#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Core/Core.hpp"
#include "Log.hpp"

namespace Piksela
{

std::shared_ptr<spdlog::logger> CoreLogger::sLogger;

void CoreLogger::Init()
{
    spdlog::set_pattern("%^[%T] %n >>> %v%$");

    sLogger = spdlog::stdout_color_mt("Core");
    sLogger->set_level(spdlog::level::trace);

    Trace("Initialized Core Logger.");
}

} // namespace Piksela