#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <cmath>

#include <arithmetic_type_tools/arithmetic_type_tools.hpp>

namespace saturating {
    using arithmetic_type_tools::min;
    using arithmetic_type_tools::max;
    using arithmetic_type_tools::clamp;
    using arithmetic_type_tools::fit_all_t;
    using arithmetic_type_tools::next_up_t;

    template <typename Tout, typename Tin>
    constexpr decltype(auto) __attribute__((const))
    round(const Tin& val) {
        // Yup, the return types differ, but it seems the `constexpr` okays this for GCC at least
        if constexpr (sizeof(Tout) > sizeof(long)) {
            return std::llround(val);
        } else {
            return std::lround(val);
        }
    }

    /**
     * Test for equality, accounting for floating point rounding differences
     */
    template <typename TA, typename TB>
    constexpr bool __attribute__((const))
    fp_safe_equals(const TA& a, const TB& b) noexcept {
        using A = std::decay_t<TA>;
        using B = std::decay_t<TB>;
        if constexpr (std::is_floating_point_v<A>) {
            if constexpr (std::is_floating_point_v<B>) {
                if constexpr (sizeof(double) > sizeof(A) || sizeof(double) > sizeof(B)) {
                    return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
                } else {
                    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
                }
            } else {
                return std::fabs(static_cast<A>(a) - static_cast<A>(b)) < std::numeric_limits<A>::epsilon();
            }
        } else {
            if constexpr (std::is_floating_point_v<B>) {
                return std::fabs(static_cast<B>(a) - static_cast<B>(b)) < std::numeric_limits<B>::epsilon();
            } else {
                return a == b;
            }
        }
    }
} // namespace saturating
