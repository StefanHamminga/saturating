#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include "./forward_decl.hpp"

namespace std {
    // Extend the standard type traits to handle the new sat types.
    template <typename T, T _min, T _max>
    class decay<saturating::type<T, _min, _max>> {
    public:
        using type = typename decay<T>::type;
    };

    template <typename T, T _min, T _max>
    struct is_unsigned<saturating::type<T, _min, _max>> {
        static constexpr decltype(auto) value = is_unsigned_v<T>;
    };

    template <typename T, T _min, T _max>
    struct is_signed<saturating::type<T, _min, _max>> {
        static constexpr decltype(auto) value = is_signed_v<T>;
    };

    template <typename T, T _min, T _max>
    struct is_integral<saturating::type<T, _min, _max>> {
        static constexpr decltype(auto) value = is_integral_v<T>;
    };

    template <typename T, T _min, T _max>
    struct is_floating_point<saturating::type<T, _min, _max>> {
        static constexpr decltype(auto) value = is_floating_point_v<T>;
    };

    template <typename T, T _min, T _max>
    struct is_arithmetic<saturating::type<T, _min, _max>> {
        static constexpr decltype(auto) value = is_arithmetic_v<T>;
    };

    template <typename T, T _min, T _max>
    class numeric_limits<saturating::type<T, _min, _max>> {
        // TODO: implement the rest of: http://en.cppreference.com/w/cpp/types/numeric_limits
    public:
        static constexpr std::decay_t<T> min()    noexcept { return _min; }
        static constexpr std::decay_t<T> lowest() noexcept { return _min; }
        static constexpr std::decay_t<T> max()    noexcept { return _max; }
    };
} // namespace std
