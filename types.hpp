/**@file
 * @brief Always saturating integer types.
 *
 * Some assumptions and notes:
 * - The operators are 'viral', adding a saturating type and any other returns another saturating type.
 * - Divide by zero clips the value to `min` or `max`
 * - Tries to avoid the normal promotion rules
 * - The separate `add`, `subtract`, etc functions can be used to define extra external operators
 *   returning saturated types.
 *
 * TODO: Further test and improve algorithms (type combination specific optimizations)
 * TODO: Add non-static functions
 * TODO: Add member `scale_to` function
 * TODO: See if there is any way to allow non integral limits for floating point based types
 * TODO: Verify floating point based types
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <cmath>

#include "./utilities.hpp"
#include "./functions.hpp"
#include "./std_saturating_awareness.hpp"

namespace saturating {
    /** Base template for a saturating integer or unsigned integer. */
    template <typename T,
              std::enable_if_t<!std::is_const_v<T>,    std::conditional_t<std::is_integral_v<T>,
                                                                          std::decay_t<T>,
                                                                          int>> MIN = std::is_integral_v<T>
                                                                                            ? static_cast<std::conditional_t<std::is_integral_v<T>,
                                                                                                                             std::decay_t<T>,
                                                                                                                             int>>(std::numeric_limits<T>::lowest())
                                                                                            : -1,
              std::enable_if_t<!std::is_volatile_v<T>, std::conditional_t<std::is_integral_v<T>,
                                                                          std::decay_t<T>,
                                                                          int>> MAX = std::is_integral_v<T>
                                                                                            ? static_cast<std::conditional_t<std::is_integral_v<T>,
                                                                                                                             std::decay_t<T>,
                                                                                                                             int>>(std::numeric_limits<T>::max())
                                                                                            : 1>
    class type {
    public:
        using value_type = std::decay_t<T>;

        static constexpr value_type min_val = MIN;
        static constexpr value_type max_val = MAX;

        /** Create a new zero-initialized saturated type. */
        constexpr type() noexcept : value{0} {}

        constexpr type(const value_type& val) noexcept : value{val} {}

        /**
         * Create a new saturating type based on a given value.
         * @param  val Initial value will *NOT* be clamped to fit `T` (use `from()`).
         */
        // template <typename U>
        // constexpr type(const U& val) noexcept : value{ static_cast<value_type>(val) } {}

        /** Conversion back to the base type */
        constexpr operator const T&() const noexcept { return value; }
        constexpr operator T&() noexcept { return value; }

        /**
         * Add a and b and store result in a new saturating type.
         * @param  a LHS
         * @param  b RHS
         * @return   New saturating type
         */
        template <typename UA, typename UB>
        static constexpr
        std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, type>
        __attribute__((const))
        add(const UA& a, const UB& b) noexcept {
            return { saturating::add<value_type, MIN, MAX, UA, UB>(a, b) };
        }

        /**
         * Subtract `b` from `a` and return a new saturating type.
         * @param  a LHS
         * @param  b RHS
         * @return   New saturating type
         */
        template <typename UA, typename UB>
        static constexpr
        std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, type>
        __attribute__((const))
        subtract(const UA& a, const UB& b) noexcept {
            return { saturating::subtract<value_type, MIN, MAX>(a, b) };
        }

        /**
         * Multiply `a` with `b` and return a new saturating type.
         * @param  a LHS
         * @param  b RHS
         * @return   New saturating type
         */
        template <typename UA, typename UB>
        static constexpr
        std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, type>
        __attribute__((const))
        multiply(const UA& a, const UB& b) noexcept {
            return { saturating::multiply<value_type, MIN, MAX>(a, b) };
        }

        /**
         * Divide `a` by `b` and return a new saturating type.
         * @param  a LHS
         * @param  b RHS
         * @return   New saturating type
         */
        template <typename UA, typename UB>
        static constexpr
        std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, type>
        __attribute__((const))
        divide(const UA& a, const UB& b) noexcept {
            return { saturating::divide<value_type, MIN, MAX>(a, b) };
        }

        constexpr auto& operator++() noexcept {
            if (value < MAX - 1) ++value;
            return *this;
        }
        constexpr auto operator++(int) noexcept {
            type<type, MIN, MAX> temp { value };
            if (value < MAX - 1) ++value;
            return temp;
        }

        constexpr auto& operator--() noexcept {
            if (value > MIN + 1) --value;
            return *this;
        }
        constexpr auto operator--(int) noexcept {
            type<type, MIN, MAX> temp { value };
            if (value > MIN + 1) --value;
            return temp;
        }

        template <typename U> constexpr auto& operator= (const U& other) noexcept { value = clamp(other); return *this; }

        template <typename U> constexpr decltype(auto) __attribute__((const)) operator+(const U& other) const noexcept { return add(value, other); }
        template <typename U> constexpr decltype(auto) __attribute__((const)) operator-(const U& other) const noexcept { return subtract(value, other); }
        template <typename U> constexpr decltype(auto) __attribute__((const)) operator*(const U& other) const noexcept { return multiply(value, other); }
        template <typename U> constexpr decltype(auto) __attribute__((const)) operator/(const U& other) const noexcept { return divide(value, other); }

        template <typename U> constexpr type __attribute__((const)) operator%(const U& other) const noexcept { return value % other; }

        template <typename U> constexpr auto& operator+=(const U& other) noexcept { value = add(value, other); return *this; }
        template <typename U> constexpr auto& operator-=(const U& other) noexcept { value = subtract(value, other); return *this; }
        template <typename U> constexpr auto& operator*=(const U& other) noexcept { value = multiply(value, other); return *this; }
        template <typename U> constexpr auto& operator/=(const U& other) noexcept { value = divide(value, other); return *this; }
        template <typename U> constexpr auto& operator%=(const U& other) noexcept { value %= other; return *this; }

        /**
         * Clamp value `val` to the base type limits. With float rounding.
         */
        template <typename U>
        static constexpr type __attribute__((const))
        clamp(const U& val) noexcept {
            if constexpr (std::is_floating_point_v<U> && std::is_integral_v<value_type>) {
                return static_cast<value_type>(minmax(MIN, round(val), MAX));
            } else {
                return static_cast<value_type>(minmax(MIN, val, MAX));
            }
        }

        /**
         * Create a new instance of this type, it's value being `val` clamped to fit `MIN` and `MAX`.
         * @param  val Initial value
         * @return     Saturating type with initial value clamped.
         */
        template <typename U>
        static constexpr type __attribute__((const)) from(const U& val) noexcept {
            return { clamp(val) };
        }

        /**
         * Scale the value of another saturating type to this one.
         * @param  val Saturating type
         * @return     Reference to this instance
         */
        template <typename U>
        constexpr auto& scale_from(const U& val) noexcept { value = type::scale_from(val); return *this; }

        /**
         * Convert one saturating type to another, scaling the value.
         * @param  val Saturating type
         * @return     New saturating type
         */
        template <typename U,
                  std::conditional_t<std::is_floating_point_v<U>, int, std::decay_t<U>> in_min,
                  std::conditional_t<std::is_floating_point_v<U>, int, std::decay_t<U>> in_max,
                  typename DISCARD = void>
        static constexpr type __attribute__((const))
        scale_from(const type<U, in_min, in_max>& val) noexcept {
            if constexpr (static_cast<fit_all_t<type, std::decay_t<U>>>(MIN) == static_cast<fit_all_t<type, std::decay_t<U>>>(in_min)) {
                if constexpr (static_cast<fit_all_t<type, std::decay_t<U>>>(MAX) == static_cast<fit_all_t<type, std::decay_t<U>>>(in_max)) {
                    return { static_cast<value_type>(val) };
                } else {
                    //TODO: Check overflow
                    return { static_cast<value_type>(((static_cast<next_up_t<T>>(val) - MIN) * in_max / MAX) + MIN) };
                }
            } else {
                if constexpr (static_cast<next_up_t<T>>(MAX) - MIN == static_cast<next_up_t<U>>(in_max) - in_min) {
                    // Range shift only
                    return { static_cast<value_type>(val + (static_cast<next_up_t<U>>(in_min) - MIN)) };
                } else {
                    if constexpr (static_cast<fit_all_t<type, std::decay_t<U>>>(MAX) == static_cast<fit_all_t<type, std::decay_t<U>>>(in_max)) {
                        return { static_cast<value_type>((val * (static_cast<next_up_t<T>>(MAX) - MIN))/(MAX - in_min)) };
                    } else {
                        auto temp = (static_cast<next_up_t<fit_all_t<T, U>>>(val) - in_min) *
                                    (static_cast<next_up_t<fit_all_t<T, U>>>(MAX) - MIN) /
                                    (static_cast<next_up_t<fit_all_t<T, U>>>(in_max) - in_min) + MIN + 1; // 1 for integer rounding
                        return { static_cast<value_type>(temp) };
                    }
                }
            }
        }

        template <typename U, typename V>
        static constexpr std::enable_if_t<std::is_floating_point_v<U> & std::is_floating_point_v<V>, type>
        __attribute__((const))
        scale_from(const U& val,
                   const V& in_min,
                   const V& in_max) noexcept
        {
            auto temp = (val - in_min) *
                        (static_cast<next_up_t<T>>(MAX) - MIN) /
                        (in_max - in_min) + MIN;
            return { static_cast<value_type>(temp + 0.5f) };
        }

        // template <typename U, typename V = int>
        // static constexpr std::enable_if_t<std::is_floating_point_v<U> && std::is_integral_v<V>, type>
        // __attribute__((const)) /**TODO: Starting the range for unsigned values from 0 instead of -1 feels optimal, at the cost of complex default behaviour */
        // scale_from(const U& val,
        //            const V& in_min = std::is_signed_v<value_type> ? -1 : 0,
        //            const V& in_max = 1) noexcept
        // {
        //     auto temp = (val - in_min) *
        //                 (static_cast<next_up_t<T>>(MAX) - MIN) /
        //                 (static_cast<next_up_t<T>>(in_max) - in_min) + MIN;
        //     return { static_cast<value_type>(temp + 0.5f) };
        // }

    private:
        T value;
    };
} // namespace saturating

#ifndef SATURATING_TYPES_h_NO_GLOBALS
using int_sat8_t    = saturating::type<int8_t>;
using uint_sat8_t   = saturating::type<uint8_t>;

using int_sat16_t   = saturating::type<int16_t>;
using uint_sat16_t  = saturating::type<uint16_t>;

using int_sat32_t   = saturating::type<int32_t>;
using uint_sat32_t  = saturating::type<uint32_t>;

using int_sat64_t   = saturating::type<int64_t>;
using uint_sat64_t  = saturating::type<uint64_t>;

#ifdef __SIZEOF_INT128__
using int_sat128_t  = saturating::type<__int128_t>;
using uint_sat128_t = saturating::type<__uint128_t>;
#endif

#if __SIZEOF_FLOAT__
using float_sat_t   = saturating::type<float>;
#endif
#if __SIZEOF_DOUBLE__
using double_sat_t  = saturating::type<double>;
#endif
#if __SIZEOF_LONG_DOUBLE__
using long_double_sat_t = saturating::type<long double>;
#endif
#endif
