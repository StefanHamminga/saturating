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
 * TODO: Further test and improve algorithms (hardware specific functions / reduce branching?)
 * TODO: Enhance interaction with floating point types
 * TODO: Add builtins for other operators
 */

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace {
    // Helpers to convert small types to the next size up, skipping the non-performant smaller upgrades.
    template <typename T, typename U,
              size_t S = (sizeof(T) > sizeof(U) ? sizeof(T) : sizeof(U)),
              bool B = std::is_unsigned_v<T>>
    struct next_up {};
    template <typename T, typename U> struct next_up<T, U, 1, false> { typedef int_fast16_t type; };
    template <typename T, typename U> struct next_up<T, U, 1, true>  { typedef uint_fast16_t type; };
    template <typename T, typename U> struct next_up<T, U, 2, false> { typedef int_fast32_t type; };
    template <typename T, typename U> struct next_up<T, U, 2, true>  { typedef uint_fast32_t type; };
    template <typename T, typename U> struct next_up<T, U, 4, false> { typedef int_fast64_t type; };
    template <typename T, typename U> struct next_up<T, U, 4, true>  { typedef uint_fast64_t type; };
#ifdef __SIZEOF_INT128__
    template <typename T, typename U> struct next_up<T, U, 8, false> { typedef __int128_t type; };
    template <typename T, typename U> struct next_up<T, U, 8, true>  { typedef __uint128_t type; };
#endif
    /** Use `next_up_t<T>` to obtain an integral type that is similar but twice the size. */
    template <typename T, typename U>
    using next_up_t = typename next_up<T, U>::type;
}

/** Base template for a saturating integer or unsigned integer. */
template <typename T,
          std::enable_if_t<std::is_integral_v<T>, T> min = std::numeric_limits<T>::min(),
          std::enable_if_t<std::is_integral_v<T>, T> max = std::numeric_limits<T>::max()>
class xint_sat_t {
public:
    typedef T type;
    typedef xint_sat_t<T> return_type; ///< This is what the operators return

    static const T min_val = min;
    static const T max_val = max;

    /** Create a new zero-initialized saturated type. */
    constexpr xint_sat_t() : value{0} {}

    /**
     * Create a new saturating type based on a given value.
     * @param  val Initial value will be clamped to fit T
     */
    template <typename U>
    constexpr xint_sat_t(const U& val) : value{clamp(val)} {}

    /** Conversion back to the base type */
    constexpr operator const T&() const { return value; }
    constexpr operator T&() { return value; }

    /**
     * Add `other` to this value and return a new saturating type.
     * @param  other Value to add to this one
     * @return       New saturating type
     */
    template <typename U>
    constexpr return_type __attribute__((pure)) add(const U& other) const {
        if constexpr (std::is_unsigned_v<T>) {
            if constexpr (std::is_unsigned_v<U>) {
                if constexpr (sizeof(T) >= sizeof(U) &&
                              sizeof(T) >= sizeof(size_t) &&
                              max == std::numeric_limits<T>::max())
                {
                    // The builtin seems work better on native and larger size T
                    T temp = 0;
                    return {
                        __builtin_add_overflow(value, static_cast<T>(other), &temp)
                            ? max
                            : temp
                    };
                } else {
                    // On smaller types however this produces fewer instructions:
                    const auto temp = static_cast<next_up_t<T, U>>(value) + other;
                    return {
                        temp > max
                        ? max
                        : static_cast<T>(temp)
                    };
                }
            } else {
                if (other < 0) {
                    if constexpr (sizeof(U) < sizeof(unsigned)) {
                        const unsigned temp = -other;
                        return {
                            (value > temp) ? static_cast<T>(value - temp) : 0
                        };
                    } else {
                        const auto temp = static_cast<std::make_unsigned_t<U>>(-other);
                        return {
                            (value > temp) ? static_cast<T>(value - temp) : 0
                        };
                    }
                } else {
                    const auto temp = static_cast<next_up_t<T, U>>(value) + other;
                    return {
                        temp > max
                            ? max
                            : static_cast<T>(temp)
                    };
                }
            }
        } else {
            if constexpr (min == std::numeric_limits<T>::min() &&
                          max == std::numeric_limits<T>::max() &&
                          std::is_signed_v<U> &&
                          sizeof(T) >= sizeof(U) &&
                          sizeof(T) >= sizeof(size_t))
            {
                T temp = 0;
                return {
                    __builtin_add_overflow(value, static_cast<T>(other), &temp)
                        ? (other < 0 ? min : max)
                        : temp
                };
            } else {
                const auto temp = static_cast<next_up_t<T, U>>(value) + other;
                return {
                    temp > max
                        ? max
                        : (temp < min
                                ? min
                                : static_cast<T>(temp))
                };
            }
        }
    }

    /**
     * Subtract `other` from this value and return a new saturating type.
     * @param  other Value to subtract from this one
     * @return       New saturating type
     */
    template <typename U>
    constexpr return_type __attribute__((pure)) subtract(const U& other) const {
        if constexpr (std::is_unsigned_v<T>) {
            if constexpr (std::is_unsigned_v<U>) {
                return {
                    other > value
                        ? 0
                        : static_cast<T>(value - other)
                };
            } else {
                if (other < 0) {
                    const auto temp = static_cast<next_up_t<T, U>>(-other) + value;
                    return {
                        temp > max
                            ? max
                            : static_cast<T>(temp)
                    };
                } else {
                    return {
                        value > other
                            ? static_cast<T>(value - other)
                            : 0
                    };
                }
            }
        } else {
            const auto temp = static_cast<next_up_t<T, U>>(value) - other;
            return {
                temp > max
                    ? max
                    : (temp < min
                            ? min
                            : static_cast<T>(temp))
            };
        }
    }

    /**
     * Multiply this value with `other` and return a new saturating type.
     * @param  other Multiplication factor
     * @return       New saturating type
     */
    template <typename U>
    constexpr return_type __attribute__((pure)) multiply(const U& other) const {
        if constexpr (std::is_unsigned_v<T>) {
            if constexpr (std::is_unsigned_v<U>) {
                if constexpr (min == std::numeric_limits<T>::min() &&
                              max == std::numeric_limits<T>::max() &&
                              sizeof(T) >= sizeof(U) &&
                              sizeof(T) >= sizeof(size_t))
                {
                    T temp = 0;
                    return {
                        __builtin_mul_overflow(value, static_cast<T>(other), &temp)
                            ? max
                            : temp
                    };
                } else {
                    const auto temp = static_cast<next_up_t<T, U>>(value) * other;
                    return {
                        temp > max
                            ? max
                            : static_cast<T>(temp)
                    };
                }
            } else {
                if (other < 0) {
                    return 0;
                } else {
                    const auto temp = static_cast<next_up_t<T, U>>(value) * other;
                    return {
                        temp > max
                            ? max
                            : static_cast<T>(temp)
                    };
                }
            }
        } else {
            if constexpr (min == std::numeric_limits<T>::min() &&
                          max == std::numeric_limits<T>::max() &&
                          std::is_signed_v<U> &&
                          sizeof(T) >= sizeof(U) &&
                          sizeof(T) >= sizeof(size_t))
            {
                T temp = 0;
                return {
                    __builtin_mul_overflow(value, static_cast<T>(other), &temp)
                        ? (other < 0 ? (value < 0 ? max : min) : (value < 0 ? min : max))
                        : temp
                };
            } else {
                const auto temp = static_cast<next_up_t<T, U>>(value) * other;
                return {
                    temp > max
                        ? max
                        : (temp < min
                                ? min
                                : static_cast<T>(temp))
                };
            }
        }
    }

    /**
     * Divide this by `other` and return a new saturating type.
     * @param  other Division factor
     * @return       New saturating type
     */
    template <typename U>
    constexpr return_type __attribute__((pure)) divide(const U& other) const {
        if (other == 0) {
            return value >= 0 ? max : min;
        } else {
            return value / other;
        }
    }

    constexpr auto& operator++() {
        if (value < max - 1) ++value;
        return *this;
    }
    constexpr auto operator++(int) {
        xint_sat_t<T> temp { value };
        if (value < max - 1) ++value;
        return temp;
    }

    constexpr auto& operator--() {
        if (value > min + 1) --value;
        return *this;
    }
    constexpr auto operator--(int) {
        xint_sat_t<T> temp { value };
        if (value > min + 1) --value;
        return temp;
    }

    template <typename U> constexpr auto& operator= (const U& other) { value = clamp(other); return *this; }

    template <typename U> constexpr decltype(auto) __attribute__((pure)) operator+(const U& other) const { return add(other); }
    template <typename U> constexpr decltype(auto) __attribute__((pure)) operator-(const U& other) const { return subtract(other); }
    template <typename U> constexpr decltype(auto) __attribute__((pure)) operator*(const U& other) const { return multiply(other); }
    template <typename U> constexpr decltype(auto) __attribute__((pure)) operator/(const U& other) const { return divide(other); }

    template <typename U> constexpr return_type __attribute__((pure)) operator%(const U& other) const { return value % other; }

    template <typename U> constexpr auto& operator+=(const U& other) { value = add(other); return *this; }
    template <typename U> constexpr auto& operator-=(const U& other) { value = subtract(other); return *this; }
    template <typename U> constexpr auto& operator*=(const U& other) { value = multiply(other); return *this; }
    template <typename U> constexpr auto& operator/=(const U& other) { value = divide(other); return *this; }
    template <typename U> constexpr auto& operator%=(const U& other) { value %= other; return *this; }

private:
    T value;
    template <typename U>
    constexpr T clamp(const U& val) const {
        if constexpr (std::is_unsigned_v<T> == std::is_unsigned_v<U> && sizeof(U) <= sizeof(T)) {
            return val;
        } else {
            return (val < min)
                        ? min
                        : (val > max
                            ? max
                            : static_cast<T>(val));
        }
    }
};

namespace std {
    // Extend the standard type traits to handle the new sat types.
    template <typename T>
    struct is_unsigned<xint_sat_t<T>> {
        static constexpr decltype(auto) value = is_unsigned_v<T>;
    };
    template <typename T>
    struct is_signed<xint_sat_t<T>> {
        static constexpr decltype(auto) value = is_signed_v<T>;
    };
    template <typename T>
    struct is_integral<xint_sat_t<T>> {
        static constexpr decltype(auto) value = is_integral_v<T>;
    };
    template <typename T>
    struct is_arithmetic<xint_sat_t<T>> {
        static constexpr decltype(auto) value = is_arithmetic_v<T>;
    };
    template <typename T, T _min, T _max>
    class numeric_limits<xint_sat_t<T, _min, _max>> {
        // TODO: implement the rest of: http://en.cppreference.com/w/cpp/types/numeric_limits
    public:
        static constexpr T min() { return _min; }
        static constexpr T lowest() { return _min; }
        static constexpr T max() { return _max; }
    };
}

typedef xint_sat_t<int8_t>   int_sat8_t;
typedef xint_sat_t<uint8_t>  uint_sat8_t;

typedef xint_sat_t<int16_t>  int_sat16_t;
typedef xint_sat_t<uint16_t> uint_sat16_t;

typedef xint_sat_t<int32_t>  int_sat32_t;
typedef xint_sat_t<uint32_t> uint_sat32_t;

typedef xint_sat_t<int64_t>  int_sat64_t;
typedef xint_sat_t<uint64_t> uint_sat64_t;

#ifdef __SIZEOF_INT128__
typedef xint_sat_t<__int128_t>  int_sat128_t;
typedef xint_sat_t<__uint128_t> uint_sat128_t;
#endif
