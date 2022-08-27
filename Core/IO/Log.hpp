#pragma once

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Core/Core.hpp"

namespace Piksela
{

class CoreLogger
{
public:
    static void Init();

    template <typename... Args>
    static void Trace(Args &&...args)
    {
        sLogger->trace(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Info(Args &&...args)
    {
        sLogger->info(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Warn(Args &&...args)
    {
        sLogger->warn(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Error(Args &&...args)
    {
        sLogger->error(std::forward<Args>(args)...);
    }

private:
    static std::shared_ptr<spdlog::logger> sLogger;
};

} // namespace Piksela