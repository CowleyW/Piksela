#pragma once

#include "Assert.hpp"
#include "IO/Log.hpp"

#include <memory.h>
#include <string>

namespace Piksela
{

template <typename T>
using ScopedPtr = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr ScopedPtr<T> CreateScopedPtr(Args &&...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr SharedPtr<T> CreateSharedPtr(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
} // namespace Piksela