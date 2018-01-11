#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <cmath>

/**
 * Saturating types and functions
 */
namespace saturating {
    template <typename T,
              std::enable_if_t<!std::is_const_v<T>,    std::conditional_t<std::is_integral_v<T>, std::decay_t<T>, int>> MIN,
              std::enable_if_t<!std::is_volatile_v<T>, std::conditional_t<std::is_integral_v<T>, std::decay_t<T>, int>> MAX>
    class type;
}
