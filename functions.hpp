#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <cmath>

#include "./utilities.hpp"

namespace saturating {
    /**
     * Add a and b and store result in a new saturating type.
     * @param  a Left hand side of operator
     * @param  b Right hand side of operator
     * @return   New saturating type
     */
    template <typename T,
              // Yes, this looks duplicative, but not using an intermediate typename enables easier custom limits
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MIN = std::is_floating_point_v<T>
                                ? -1
                                : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::lowest(),
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MAX = std::is_floating_point_v<T>
                                ? 1
                                : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::max(),
              typename UA,
              typename UB>
    constexpr std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, std::decay_t<T>>
    __attribute__((const))
    add(const UA& a, const UB& b) noexcept {
        if constexpr (std::is_floating_point_v<T>) {
            if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
                return static_cast<std::decay_t<T>>(minmax(MIN, a + b, MAX));
            } else {
                using TC = fit_all_t<UA, UB>;
                if constexpr (MIN == std::numeric_limits<TC>::lowest() && MAX == std::numeric_limits<TC>::max()) {
                    TC temp = 0;
                    if constexpr (std::is_unsigned_v<TC>) {
                        return {
                            __builtin_add_overflow(static_cast<TC>(a), static_cast<TC>(b), &temp)
                                ? MAX
                                : temp
                        };
                    } else {
                        return {
                            __builtin_add_overflow(static_cast<TC>(a), static_cast<TC>(b), &temp)
                                ? (static_cast<TC>(b) > static_cast<TC>(a) ? MIN : MAX)
                                : temp
                        };
                    }
                } else {
                    using TO = next_up_t<TC>;
                    return static_cast<std::decay_t<T>>(minmax(MIN, static_cast<TO>(a) + static_cast<TO>(b), MAX));
                }
            }
        } else {
            if constexpr (std::is_floating_point_v<UA>) {
                if constexpr (std::is_floating_point_v<UB>) {
                    return static_cast<std::decay_t<T>>(minmax(MIN, round<T>(a + b), MAX));
                } else {
                    const auto temp = round<T>(a);
                    using TO = next_up_t<fit_all_t<UB, decltype(temp)>>;
                    return static_cast<std::decay_t<T>>(minmax(MIN, static_cast<TO>(temp) + static_cast<TO>(b), MAX));
                }
            } else {
                if constexpr (std::is_floating_point_v<UB>) {
                    const auto temp = round<T>(b);
                    using TO = next_up_t<fit_all_t<UA, decltype(temp)>>;
                    return static_cast<std::decay_t<T>>(minmax(MIN, static_cast<TO>(a) + static_cast<TO>(temp), MAX));
                } else {
                    if constexpr (MIN == std::numeric_limits<T>::lowest() && MAX == std::numeric_limits<T>::max() && std::is_same_v<T, fit_all_t<T, UA, UB>>) {
                        T temp = 0;
                        if constexpr (std::is_unsigned_v<T>) {
                            return {
                                __builtin_add_overflow(static_cast<T>(a), static_cast<T>(b), &temp)
                                    ? MAX
                                    : temp
                            };
                        } else {
                            return {
                                __builtin_add_overflow(static_cast<T>(a), static_cast<T>(b), &temp)
                                    ? (static_cast<T>(b) > static_cast<T>(a) ? MIN : MAX)
                                    : temp
                            };
                        }
                    } else {
                        using TO = next_up_t<fit_all_t<UA, UB>>;
                        return static_cast<std::decay_t<T>>(minmax(MIN, static_cast<TO>(a) + static_cast<TO>(b), MAX));
                    }
                }
            }
        }
    }

    /**
     * Add @param{val} to @param{out}, returning if overflow occured.
     * @param  out Ouput variable
     * @param  val Value to add
     * @param  MIN Custom overflow minimum (always integral)
     * @param  MAX Custom overflow maximum (always integral)
     * @return     Overflow?
     */
    template <typename T, typename U>
    constexpr bool add_to(T& out,
                          const U& val,
                          std::conditional_t<std::is_floating_point_v<T>, int, T> MIN = std::is_floating_point_v<T> ? (int)-1 : std::numeric_limits<T>::lowest(),
                          std::conditional_t<std::is_floating_point_v<T>, int, T> MAX = std::is_floating_point_v<T> ?  (int)1 : std::numeric_limits<T>::max())
    {
        if constexpr (std::is_floating_point_v<T>) {
            out += static_cast<std::decay_t<T>>(val);
            if (out > MAX) {
                out = MAX;
                return true;
            } else if (out < MIN) {
                out = MIN;
                return true;
            } else {
                return false;
            }
        } else {
            if constexpr (MIN == std::numeric_limits<T>::lowest() &&
                          MAX == std::numeric_limits<T>::max() &&
                          std::is_same_v<T, fit_all_t<T, U>>)
            {
                if (__builtin_add_overflow(out, static_cast<T>(val), &out)) {
                    if constexpr (std::is_unsigned_v<U>) {
                        out = MAX;
                    } else {
                        out = static_cast<T>(val) > out ? MAX : MIN;
                    }
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    /**
     * Subtract `b` from `a` and return a new saturating type.
     * @param  a Left hand side of operator
     * @param  b Right hand side of operator
     * @return   New saturating type
     */
    template <typename T,
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MIN = std::is_floating_point_v<T>
                            ? -1
                            : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::lowest(),
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MAX = std::is_floating_point_v<T>
                            ? 1
                            : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::max(),
              typename UA,
              typename UB>
    constexpr std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, std::decay_t<T>>
    __attribute__((const))
    subtract(const UA& a, const UB& b) noexcept {
        using TO = next_up_t<fit_all_t<UA, UB>>;
        if constexpr (std::is_floating_point_v<T>) {
            if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
                return minmax(MIN, a - b, MAX);
            } else {
                return minmax(MIN, static_cast<TO>(a) - b, MAX);
            }
        } else {
            if constexpr (std::is_floating_point_v<UA>) {
                if constexpr (std::is_floating_point_v<UB>) {
                    return static_cast<std::decay_t<T>>(minmax(MIN, round<T>(a - b), MAX));
                } else {
                    return static_cast<std::decay_t<T>>(minmax(MIN, round<T>(a - b), MAX));
                }
            } else {
                if constexpr (std::is_floating_point_v<UB>) {
                    return static_cast<std::decay_t<T>>(minmax(MIN, round<T>(a - b), MAX));
                } else {
                    return static_cast<std::decay_t<T>>(minmax(MIN, static_cast<TO>(a) - static_cast<TO>(b), MAX));
                }
            }
        }
    }

    template <typename T,
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MIN = std::is_floating_point_v<T>
                            ? -1
                            : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::lowest(),
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MAX = std::is_floating_point_v<T>
                            ? 1
                            : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::max(),
              typename UA,
              typename UB>
    constexpr std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, std::decay_t<T>>
    __attribute__((const))
    multiply(const UA& a, const UB& b) noexcept {
        using TO = next_up_t<fit_all_t<UA, UB>>;
        if constexpr (std::is_floating_point_v<T>) {
            if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
                return minmax(MIN, a * b, MAX);
            } else {
                return minmax(MIN, static_cast<TO>(a) * b, MAX);
            }
        } else {
            if constexpr (std::is_floating_point_v<UA>) {
                if constexpr (std::is_floating_point_v<UB>) {
                    return minmax(MIN, round<T>(a * b), MAX);
                } else {
                    return minmax(MIN, round<T>(a * b), MAX);
                }
            } else {
                if constexpr (std::is_floating_point_v<UB>) {
                    return minmax(MIN, round<T>(a * b), MAX);
                } else {
                    return minmax(MIN, static_cast<TO>(a) * static_cast<TO>(b), MAX);
                }
            }
        }
    }

    template <typename T,
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MIN = std::is_floating_point_v<T>
                            ? -1
                            : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::lowest(),
              std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>
                    MAX = std::is_floating_point_v<T>
                            ? 1
                            : (std::conditional_t<std::is_floating_point_v<T>, int, std::decay_t<T>>)std::numeric_limits<T>::max(),
              typename UA,
              typename UB>
    constexpr std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, std::decay_t<T>>
    __attribute__((const))
    divide(const UA& a, const UB& b) noexcept {
        using TO = next_up_t<fit_all_t<UA, UB>>;
        if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
            if constexpr (std::is_floating_point_v<T>) {
                return static_cast<std::decay_t<T>>(minmax(MIN, a / b, MAX));
            } else {
                return static_cast<std::decay_t<T>>(minmax(MIN, round<T>(a / b), MAX));
            }
        } else {
            return static_cast<std::decay_t<T>>(minmax(MIN, ((a < 0) ^ (b < 0)) ? ((a - b/2)/b) : ((a + b/2)/b), MAX));
        }
    }

    //TODO: increments, pow, square, sqrt, etc...

    template <typename UA, typename UB, typename T>
    constexpr void add(const UA& a, const UB& b, T& out) noexcept { out = add<T>(a, b); }
    template <typename UA, typename UB, typename T>
    constexpr void subtract(const UA& a, const UB& b, T& out) noexcept { out = subtract<T>(a, b); }
    template <typename UA, typename UB, typename T>
    constexpr void multiply(const UA& a, const UB& b, T& out) noexcept { out = multiply<T>(a, b); }
    template <typename UA, typename UB, typename T>
    constexpr void divide(const UA& a, const UB& b, T& out) noexcept { out = divide<T>(a, b); }
} // namespace saturating
