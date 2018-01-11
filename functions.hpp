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

    // static constexpr
    // std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, type>
    // __attribute__((const))
    // subtract(const UA& a, const UB& b) noexcept {
    //     using TO = next_up_t<UA, UB>;
    //     if constexpr (std::is_floating_point_v<value_type>) {
    //         if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
    //             return { minmax(MIN, a - b, MAX) };
    //         } else {
    //             return { minmax(MIN, static_cast<TO>(a) - b, MAX) };
    //         }
    //     } else {
    //         if constexpr (std::is_floating_point_v<UA>) {
    //             if constexpr (std::is_floating_point_v<UB>) {
    //                 return { minmax(MIN, round<T>(a - b), MAX) };
    //             } else {
    //                 return subtract(round<T>(a), b);
    //             }
    //         } else {
    //             if constexpr (std::is_floating_point_v<UB>) {
    //                 return subtract(a, round<T>(b));
    //             } else {
    //                 return {
    //                     minmax(MIN, static_cast<TO>(a) - static_cast<TO>(b), MAX)
    //                 };
    //             }
    //         }
    //     }
    //
    //     // if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
    //     //     if constexpr (std::is_floating_point_v<value_type>) {
    //     //         return {
    //     //             clamp(a - b)
    //     //         };
    //     //     } else {
    //     //         return {
    //     //             clamp(round<T>(a - b))
    //     //         };
    //     //     }
    //     // } else { // Both arguments and our base type are integral
    //     //     return {
    //     //         clamp(static_cast<next_up_t<UA, UB>>(a) - static_cast<fit_all_t<UA, UB>>(b))
    //     //     };
    //     //     // if constexpr (MIN == std::numeric_limits<value_type>::lowest() &&
    //     //     //               MAX == std::numeric_limits<value_type>::max() &&
    //     //     //               std::is_signed_v<value_type> == std::is_signed_v<UA> &&
    //     //     //               std::is_signed_v<value_type> == std::is_signed_v<UB> &&
    //     //     //               sizeof(value_type) >= sizeof(UA) &&
    //     //     //               sizeof(value_type) >= sizeof(UB))
    //     //     // {
    //     //     //     type temp = 0;
    //     //     //     return {
    //     //     //         __builtin_sub_overflow(static_cast<value_type>(a), static_cast<value_type>(b), &temp)
    //     //     //             ? (b > a ? MIN : MAX)
    //     //     //             : temp
    //     //     //     };
    //     //     // } else {
    //     //     //     if constexpr (std::is_signed_v<value_type> || std::is_signed_v<UA> || std::is_signed_v<UB>) {
    //     //     //         return {
    //     //     //             clamp(static_cast<std::make_signed_t<next_up_t<UA, UB>>>(a) - b)
    //     //     //         };
    //     //     //     } else {
    //     //     //         return {
    //     //     //             b > a
    //     //     //                 ? 0
    //     //     //                 : clamp(static_cast<next_up_t<UA, UB>>(a) - b)
    //     //     //         };
    //     //     //     }
    //     //     // }
    //     // }
    // }
    //
    // /**
    //  * Multiply `a` with `b` and return a new saturating type.
    //  * @param  a Left hand side of operator
    //  * @param  b Right hand side of operator
    //  * @return   New saturating type
    //  */
    // template <typename UA, typename UB>
    // static constexpr
    // std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, type>
    // __attribute__((const))
    // multiply(const UA& a, const UB& b) noexcept {
    //     if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
    //         if constexpr (std::is_floating_point_v<value_type>) {
    //             return {
    //                 clamp(a * b)
    //             };
    //         } else {
    //             return {
    //                 clamp(round<T>(a * b))
    //             };
    //         }
    //     } else {
    //         // if constexpr (MIN == std::numeric_limits<value_type>::lowest() &&
    //         //               MAX == std::numeric_limits<value_type>::max() &&
    //         //               std::is_signed_v<value_type> == std::is_signed_v<UA> &&
    //         //               std::is_signed_v<value_type> == std::is_signed_v<UB> &&
    //         //               sizeof(value_type) >= sizeof(UA) &&
    //         //               sizeof(value_type) >= sizeof(UB))
    //         // {
    //         //     type temp = 0;
    //         //     return {
    //         //         __builtin_mul_overflow(static_cast<value_type>(a), static_cast<value_type>(b), &temp)
    //         //             ? ((a < 0) == (b < 0)
    //         //                     ? MAX
    //         //                     : MIN)
    //         //             : temp
    //         //     };
    //         // } else {
    //             return {
    //                 clamp(static_cast<next_up_t<UA, UB>>(a) * b)
    //             };
    //         // }
    //     }
    // }
    //
    // /**
    //  * Divide `a` by `b` and return a new saturating type.
    //  * @param  a Left hand side of operator
    //  * @param  b Right hand side of operator
    //  * @return   New saturating type
    //  */
    // template <typename UA, typename UB>
    // static constexpr
    // std::enable_if_t<std::is_arithmetic_v<UA> && std::is_arithmetic_v<UB>, type>
    // __attribute__((const))
    // divide(const UA& a, const UB& b) noexcept {
    //     if constexpr (std::is_floating_point_v<UA> || std::is_floating_point_v<UB>) {
    //         if constexpr (std::is_floating_point_v<value_type>) {
    //             return {
    //                 clamp(static_cast<value_type>(a) / static_cast<value_type>(b))
    //             };
    //         } else {
    //             return {
    //                 clamp(static_cast<fit_all_t<UA, UB>>(a) / static_cast<fit_all_t<UA, UB>>(b))
    //             };
    //         }
    //     } else {
    //         if constexpr (sizeof(value_type) >= sizeof(UA)) {
    //             return { static_cast<value_type>(a / b) };
    //         } else {
    //             return { clamp(a / b) };
    //         }
    //     }
    // }
} // namespace saturating
